#include <hardware/structs/i2c.h>
#include <hardware/structs/io_bank0.h>
#include <stdbool.h>

#include <hardware/adc.h>
#include <hardware/gpio.h>
#include <hardware/i2c.h>
#include <pico.h>
#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <pico/time.h>

#include "drivers/bh1750.h"
#include "drivers/s12sd.h"
#include "pw_cc.h"
#include "pw_cfg.h"
#include "pw_log.h"

static bh1750_state_t bh1750_state = { 0 };

PW_ATTR_ALWAYS_INLINE
inline static void init(void)
{
	uint bh1750_i2c_hz_actual;

	pw_log_level_set(LOG_LEVEL_TRACE);
	stdio_init_all();
	sleep_ms(10000);
	pw_log(LOG_LEVEL_TRACE, "Initialized stdio.");

	adc_init();
	s12sd_init(S12SD_GPIO_PIN);
	pw_log(LOG_LEVEL_TRACE, "Initialized ADC and S12SD peripheral.");

	gpio_set_function(BH1750_GPIO_PIN_I2C_SDA, GPIO_FUNC_I2C);
	gpio_set_function(BH1750_GPIO_PIN_I2C_SCL, GPIO_FUNC_I2C);
	bh1750_i2c_hz_actual =
		i2c_init(&BH1750_I2C_INST, BH1750_I2C_SPEED_MAX_HZ);
	pw_log(LOG_LEVEL_TRACE,
	       "Initialized I2C%u for BH1750 with a preferred baudrate of %u.",
	       BH1750_I2C_INST_N, BH1750_I2C_SPEED_MAX_HZ);
	if (bh1750_i2c_hz_actual > BH1750_I2C_SPEED_MAX_HZ) {
		bh1750_i2c_hz_actual = i2c_set_baudrate(&BH1750_I2C_INST,
							I2C_STANDARD_MODE_HZ);
		pw_log(LOG_LEVEL_WARN,
		       "i2c_init(I2C%u) returned baudrate higher than BH1750 supports. Using standard mode baudrate.",
		       BH1750_I2C_INST_N);
	}
	bh1750_init(&bh1750_state, &BH1750_I2C_INST);
	// This only fails if there is an active measurement, no need to verify
	(void)bh1750_mode_set(&bh1750_state, BH1750_MODE_HRES1_ONCE);
	pw_log(LOG_LEVEL_TRACE,
	       "Initialized I2C%u for BH1750 with an actual baudrate of %u and in mode %d.",
	       BH1750_I2C_INST_N, bh1750_i2c_hz_actual, BH1750_MODE_HRES1_ONCE);
}

int main()
{
	init();

	while (true) {
		uint32_t uv_index_centi;
		uint32_t lux_centi;
		uint64_t bh1750_mt_us;

		float uv_index;
		float lux;

		bh1750_mt_us = bh1750_measurement_start(&bh1750_state);
		uv_index_centi = s12sd_read_uv_index_centi(S12SD_GPIO_PIN);
		uv_index = (float)uv_index_centi / 100.0f;
		pw_log(LOG_LEVEL_INFO, "UV Index: %0.2f", uv_index);

		if (bh1750_mt_us != 0) {
			sleep_us(bh1750_mt_us);
			lux_centi = bh1750_read_lux_centi(&bh1750_state);
			lux = (float)lux_centi / 100.0f;
			pw_log(LOG_LEVEL_INFO, "Lux: %0.2f", lux);
		}

		sleep_ms(2000);
	}
}
