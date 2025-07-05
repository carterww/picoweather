
#include <hardware/i2c.h>
#include <pico/error.h>
#include <pico/time.h>
#include <pico/types.h>

#include "bh1750.h"
#include "pw_log.h"

#define BH1750_MT_REG_DEFAULT (0x45)
#define BH1750_MT_REG_MIN (0x1f)
#define BH1750_MT_REG_MAX (0xfe)

// See bh1750_is_mode_* functions for how this is used
#define BH1750_MODE_ONCE_BITMAP (0b111000)

enum bh1750_cmd {
	BH1750_CMD_POWER_DOWN = 0x00,
	BH1750_CMD_POWER_ON = 0x01,
	BH1750_CMD_RESET = 0x07,
	BH1750_CMD_HRES1_CONT = 0x10,
	BH1750_CMD_HRES2_CONT = 0x11,
	BH1750_CMD_LRES_CONT = 0x13,
	BH1750_CMD_HRES1_ONCE = 0x20,
	BH1750_CMD_HRES2_ONCE = 0x21,
	BH1750_CMD_LRES_ONCE = 0x23,
	BH1750_CMD_MT_CHANGE_HIGH = 0x40,
	BH1750_CMD_MT_CHANGE_LOW = 0x60,
};

static const int64_t MODE_TO_DEFAULT_MT_US[] = {
	[BH1750_MODE_HRES1_CONT] = 120000, [BH1750_MODE_HRES2_CONT] = 120000,
	[BH1750_MODE_LRES_CONT] = 16000,   [BH1750_MODE_HRES1_ONCE] = 120000,
	[BH1750_MODE_HRES2_ONCE] = 120000, [BH1750_MODE_LRES_ONCE] = 16000
};

static const uint8_t MODE_TO_READ_CMD[] = {
	[BH1750_MODE_HRES1_CONT] = BH1750_CMD_HRES1_CONT,
	[BH1750_MODE_HRES2_CONT] = BH1750_CMD_HRES2_CONT,
	[BH1750_MODE_LRES_CONT] = BH1750_CMD_LRES_CONT,
	[BH1750_MODE_HRES1_ONCE] = BH1750_CMD_HRES1_ONCE,
	[BH1750_MODE_HRES2_ONCE] = BH1750_CMD_HRES2_ONCE,
	[BH1750_MODE_LRES_ONCE] = BH1750_CMD_LRES_ONCE
};

inline static int bh1750_is_mode_once(bh1750_mode_t mode)
{
	return ((1 << mode) & BH1750_MODE_ONCE_BITMAP);
}

inline static uint64_t bh1750_mt_us_calc(bh1750_mode_t mode, uint8_t mtreg)
{
	int64_t mt_us_default;
	int64_t num;
	int64_t mt_us;

	mt_us_default = MODE_TO_DEFAULT_MT_US[mode];
	// Add the denominator - 1 to ensure we round up on division instead of
	// round down.
	num = (int64_t)mtreg * mt_us_default + BH1750_MT_REG_DEFAULT - 1;

	mt_us = num / BH1750_MT_REG_DEFAULT;
	return mt_us;
}

static int bh1750_i2c_write_raw(bh1750_state_t *state, const uint8_t *src,
				size_t len)
{
	int nbytes;

	nbytes = i2c_write_blocking(state->i2c, BH1750_I2C_ADDRESS, src, len,
				    false);
	if (nbytes == PICO_ERROR_GENERIC) {
		pw_log(LOG_LEVEL_ERROR,
		       "[%x] Failed to write to BH1750. Address not acknowledged.",
		       src[0]);
	} else if ((size_t)nbytes != len) {
		pw_log(LOG_LEVEL_ERROR,
		       "[%x] Wrote %d bytes to the BH1750 but %u was expected.",
		       src[0], nbytes, len);
	}
	return nbytes;
}

static int bh1750_i2c_read_raw(bh1750_state_t *state, uint8_t *dest, size_t len)
{
	int nbytes;

	nbytes = i2c_read_blocking(state->i2c, BH1750_I2C_ADDRESS, dest, len,
				   false);
	if (nbytes == PICO_ERROR_GENERIC) {
		pw_log(LOG_LEVEL_ERROR,
		       "Failed to read from BH1750. Address not acknowledged.");
	} else if ((size_t)nbytes != len) {
		pw_log(LOG_LEVEL_ERROR,
		       "Reaad %d bytes from the BH1750 but %u was expected.",
		       nbytes, len);
	}
	return nbytes;
}

static void bh1750_power_down(bh1750_state_t *state)
{
	static const uint8_t power_down_cmd = BH1750_CMD_POWER_DOWN;

	(void)bh1750_i2c_write_raw(state, &power_down_cmd, 1);
}

static void bh1750_power_up(bh1750_state_t *state)
{
	static const uint8_t power_up_cmd = BH1750_CMD_POWER_ON;

	(void)bh1750_i2c_write_raw(state, &power_up_cmd, 1);
}

void bh1750_init(bh1750_state_t *state, i2c_inst_t *i2c_bus)
{
	state->i2c = i2c_bus;
	state->measurement_start_last = 0;
	state->mode = BH1750_MODE_HRES1_ONCE;
	state->mt_us = MODE_TO_DEFAULT_MT_US[state->mode];
	state->measurement_active = false;
	state->mtreg = BH1750_MT_REG_DEFAULT;
}

bool bh1750_mode_set(bh1750_state_t *state, bh1750_mode_t mode_new)
{
	int mode_old_once;
	int mode_new_once;
	// Don't allow mode to change when measurement is active. It messes
	// with bh1750_is_ready()
	if (state->measurement_active == true) {
		return false;
	}
	mode_old_once = bh1750_is_mode_once(state->mode);
	mode_new_once = bh1750_is_mode_once(mode_new);
	// If we go from once -> cont mode we need to power device on now.
	// The read functions only do it for once mode
	if (mode_old_once && !mode_new_once) {
		bh1750_power_up(state);
		// Similar reasoning as above but vice versa. We want to power down
		// here to conserve power
	} else if (!mode_old_once && mode_new_once) {
		bh1750_power_down(state);
	}
	state->mode = mode_new;
	state->mt_us = bh1750_mt_us_calc(mode_new, state->mtreg);
	return true;
}

uint64_t bh1750_measurement_start(bh1750_state_t *state)
{
	int nbytes;
	uint8_t measure_cmd;

	if (state->measurement_active == true) {
		return 0;
	}

	// If the device is in once mode we need to power it on
	// first.
	if (bh1750_is_mode_once(state->mode)) {
		bh1750_power_up(state);
	}

	measure_cmd = MODE_TO_READ_CMD[state->mode];
	nbytes = bh1750_i2c_write_raw(state, &measure_cmd, 1);
	if (nbytes != 1) {
		pw_log(LOG_LEVEL_ERROR, "Failed to start measurement.");
		return 0;
	}
	state->measurement_active = true;
	return (uint64_t)state->mt_us;
}

uint16_t bh1750_read_raw(bh1750_state_t *state)
{
	int nbytes;
	uint8_t measure_cmd;
	uint8_t buffer[2];
	uint16_t raw;
	bool power_on_explicit = false;

	measure_cmd = MODE_TO_READ_CMD[state->mode];
	nbytes = bh1750_i2c_write_raw(state, &measure_cmd, 1);

	// The device may be powered down already. We should power it up then
	// try again
	if (nbytes != 1 && bh1750_is_mode_once(state->mode)) {
		pw_log(LOG_LEVEL_INFO,
		       "bh1750_read_raw() failed after first write. Powering on then trying agian.");
		power_on_explicit = true;
		bh1750_power_up(state);
		nbytes = bh1750_i2c_write_raw(state, &measure_cmd, 1);
		// Who knows
		if (nbytes != 1) {
			pw_log(LOG_LEVEL_FATAL,
			       "BH1750 failed to accept ONCE command even after being explicitly powered on.");
			bh1750_power_down(state);
			return 0;
		}
	}

	nbytes = bh1750_i2c_read_raw(state, buffer, 2);
	if (power_on_explicit == true) {
		bh1750_power_down(state);
	}
	if (nbytes != 2) {
		pw_log(LOG_LEVEL_ERROR,
		       "Failed to read 2 bytes from BH1750 after issuing command %x.",
		       measure_cmd);
		return 0;
	}

	state->measurement_active = false;
	raw = ((uint16_t)buffer[0] << 8) | buffer[1];
	return raw;
}

uint32_t bh1750_read_lux_centi(bh1750_state_t *state)
{
	uint16_t raw;

	raw = bh1750_read_raw(state);
	return bh1750_raw_to_lux_centi(state, raw);
}

uint64_t bh1750_time_until_ready(bh1750_state_t *state)
{
	int64_t elapsed_us;
	int64_t remaining_us;

	// I feel like this is a vacuously true statement
	if (state->measurement_active == false) {
		return 0;
	}
	elapsed_us = absolute_time_diff_us(state->measurement_start_last,
					   get_absolute_time());
	remaining_us = state->mt_us - elapsed_us;
	// This shouldn't be possible but better to handle it
	if (remaining_us <= 0) {
		return 0;
	}
	return remaining_us;
}

bool bh1750_is_ready(bh1750_state_t *state)
{
	return bh1750_time_until_ready(state) == 0;
}

uint16_t bh1750_read_raw_blocking(bh1750_state_t *state);
uint32_t bh1750_read_lux_blocking(bh1750_state_t *state);

uint32_t bh1750_raw_to_lux_centi(bh1750_state_t *state, uint16_t raw)
{
	return ((uint32_t)raw * BH1750_MT_REG_DEFAULT * 1000) /
	       (12 * state->mtreg);
}

bool bh1750_mtreg_set(bh1750_state_t *state, uint8_t mtreg_new)
{
	if (mtreg_new < BH1750_MT_REG_MIN || mtreg_new > BH1750_MT_REG_MAX) {
		return false;
	}
	state->mt_us = bh1750_mt_us_calc(state->mode, mtreg_new);
	state->mtreg = mtreg_new;
	return true;
}

uint8_t bh1750_mt_ms_set(bh1750_state_t *state, uint32_t mt_ms_new);

void bh1750_reset(bh1750_state_t *state);
