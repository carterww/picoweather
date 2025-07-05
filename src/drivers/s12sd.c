#include <stdint.h>

#include <hardware/gpio.h>
#include <hardware/adc.h>
#include <pico/types.h>

#include "pw_cfg.h"
#include "s12sd.h"

inline static uint gpio_pin_to_adc_input(uint gpio)
{
	return gpio - ADC0_GPIO_PIN;
}

void s12sd_init(uint gpio)
{
	adc_gpio_init(gpio);
#if ADC_NDEVICES <= 1
	adc_select_input(gpio_pin_to_adc_input(gpio));
#endif
}

uint16_t s12sd_read_raw(uint gpio)
{
#if ADC_NDEVICES > 1
	adc_select_input(gpio_pin_to_adc_input(gpio));
#endif
	return adc_read();
}

uint32_t s12sd_read_uv_index_centi(uint gpio)
{
	/* The docs state: UV Index = Vo / 0.1V.
	 *
	 * To avoid floating point math, everything is done in mV.
	 *   UV Index = Vo / 0.1 = Vo_mV / 100 => UV Index * 100 = Vo_mV.
	 *
	 * So Vo_mV can be returned directly as the centi-UV index.
	 */
	uint16_t raw;

	raw = s12sd_read_raw(gpio);
	return s12sd_raw_to_uv_index_centi(raw);
}

uint32_t s12sd_raw_to_uv_index_centi(uint16_t raw)
{
	/* The docs state: UV Index = Vo / 0.1V.
	 *
	 * To avoid floating point math, everything is done in mV.
	 *   UV Index = Vo / 0.1 = Vo_mV / 100 => UV Index * 100 = Vo_mV.
	 *
	 * So Vo_mV can be returned directly as the centi-UV index.
	 */
	uint32_t vo_mv = (raw * ADC_VREF_MV) / ADC_READ_MAX;
	return vo_mv;
}
