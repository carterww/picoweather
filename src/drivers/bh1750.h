#ifndef _PICOWEATHER_DRIVERS_BH1750_H
#define _PICOWEATHER_DRIVERS_BH1750_H

#include <stdbool.h>
#include <stdint.h>

#include <hardware/i2c.h>
#include <pico/types.h>

/* 
 *
 * Adafruit Sheet: https://cdn-learn.adafruit.com/downloads/pdf/adafruit-bh1750-ambient-light-sensor.pdf
 * Datasheet: https://www.mouser.com/datasheet/2/348/bh1750fvi-e-186247.pdf
 */

/* If the ADDR pin is pulled high the I2C address is
 * different then if it were low. My breakout is low
 * by default.
 */
#define BH1750_I2C_ADDRESS_H (0x5c)
#define BH1750_I2C_ADDRESS_L (0x23)
#define BH1750_I2C_ADDRESS BH1750_I2C_ADDRESS_L

#define BH1750_I2C_SPEED_MAX_HZ (400000)
#define BH1750_I2C_SPEED (100000)

enum bh1750_mode {
	BH1750_MODE_HRES1_CONT = 0,
	BH1750_MODE_HRES2_CONT,
	BH1750_MODE_LRES_CONT,
	BH1750_MODE_HRES1_ONCE,
	BH1750_MODE_HRES2_ONCE,
	BH1750_MODE_LRES_ONCE,
};
typedef enum bh1750_mode bh1750_mode_t;

struct bh1750_state {
	i2c_inst_t *i2c;
	absolute_time_t measurement_start_last;
	bh1750_mode_t mode;
	int64_t mt_us;
	bool measurement_active;
	uint8_t mtreg;
};
typedef struct bh1750_state bh1750_state_t;

void bh1750_init(bh1750_state_t *state, i2c_inst_t *i2c_bus);
bool bh1750_mode_set(bh1750_state_t *state, bh1750_mode_t mode_new);

uint64_t bh1750_measurement_start(bh1750_state_t *state);
uint16_t bh1750_read_raw(bh1750_state_t *state);
uint32_t bh1750_read_lux_centi(bh1750_state_t *state);
uint64_t bh1750_time_until_ready(bh1750_state_t *state);
bool bh1750_is_ready(bh1750_state_t *state);

uint16_t bh1750_read_raw_blocking(bh1750_state_t *state);
uint32_t bh1750_read_lux_centi_blocking(bh1750_state_t *state);

uint32_t bh1750_raw_to_lux_centi(bh1750_state_t *state, uint16_t raw);

bool bh1750_mtreg_set(bh1750_state_t *state, uint8_t mtreg_new);
uint8_t bh1750_mt_ms_set(bh1750_state_t *state, uint32_t mt_ms_new);

void bh1750_reset(bh1750_state_t *state);

#endif /* _PICOWEATHER_DRIVERS_BH1750_H */
