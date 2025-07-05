#ifndef _PICOWEATHER_DRIVERS_S12SD_H
#define _PICOWEATHER_DRIVERS_S12SD_H

#include <stdint.h>

#include <pico/types.h>

/* The GUVA-S12SD breakout is a UV light sensor that uses
 * an analog signal. It uses a UV photodiode to detect light in the
 * 240-370nm range.
 *
 * Vo = 4.3 * Diode-Current(uA)
 * UV Index = Vo / 0.1V
 *
 * Adafruit Sheet: https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/2284/1918_Web.pdf
 * Datasheet: https://cdn-shop.adafruit.com/datasheets/1918guva.pdf
 */

void s12sd_init(uint gpio);
uint16_t s12sd_read_raw(uint gpio);
uint32_t s12sd_read_uv_index_centi(uint gpio);
uint32_t s12sd_raw_to_uv_index_centi(uint16_t raw);

#endif /* _PICOWEATHER_DRIVERS_S12SD_H */
