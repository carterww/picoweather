#ifndef _PICOWEATHER_DRIVERS_SGP30_H
#define _PICOWEATHER_DRIVERS_SGP30_H

#include <stdint.h>

/* The SGP30 is an indoor air quality sensor that communicates over I2C.
 * It measures H2 and ethanol and calculates the total volatile organic compounds
 * (TVOC) and the carbon dioxide equivalent (eCO2). All commands and data are 16
 * bits wide.
 *
 * I2C specs:
 * - Fast Mode (400 kHz max)
 * - 0x58 Address
 *
 * Random things:
 * - Cannot receive commands until 0.4-0.6ms after startup.
 * - No communication is possible after issuing the measurement command.
 * - Data is passed in chunks of 2 bytes + 1 byte CRC checksum.
 *
 * Datasheet: https://sensirion.com/media/documents/984E0DD5/61644B8B/Sensirion_Gas_Sensors_Datasheet_SGP30.pdf
 */

#define SGP30_I2C_ADDRESS ((uint8_t)0x58)
#define SGP30_CRC8_INIT ((uint8_t)0xFF)
#define SGP30_CRC8_POLY ((uint8_t)0x31)
#define SGP30_CRC8_XOR ((uint8_t)0x00)

/* Each command is 16 bits.
 * The sgp30_command_iaq_init command has to be sent after a power-up or soft
 * reset. sgp30_command_measure_iaq returns fixed values for 15s after the init
 * command.
 *
 * After the init command, sgp30_command_measure_iaq must be sent every 1 second.
 * Measure will return fixed values of 400ppm CO2eq and 0 ppb TVOC for the first
 * 15 seconds.
 *
 * CO2eq (ppm) is sent first and TVOC(ppb) is sent second. Each value is 2 bytes +
 * 1 byte CRC checksum.
 */
enum sgp30_command_enum {
	sgp30_command_iaq_init = 0x2003,
	sgp30_command_measure_iaq = 0x2008,
	sgp30_command_get_iaq_baseline = 0x2015,
	sgp30_command_set_iaq_baseline = 0x201e,
	sgp30_command_set_absolute_humidity = 0x2061,
	sgp30_command_measure_test = 0x2032,
	sgp30_command_get_feature_set = 0x202f,
	sgp30_command_measure_raw = 0x2050,
	sgp30_command_get_tvoc_inceptive_baseline = 0x20b3,
	sgp30_command_set_tvoc_baseline = 0x2077,
};

struct sgp30_measure_result {
	uint16_t co2eq_ppm; /* 400-60,000 */
	uint16_t tvoc_ppb; /* 0-60,000 */
};

struct sgp30_measure_raw_result {
	uint16_t ethanol_ppm; /* 0-1,000 */
	uint16_t h2_ppm; /* 0-1,000 */
};

struct sgp30_iaq_baseline {
	uint16_t co2eq_iaq_baseline;
	uint16_t tvoc_iaq_baseline;
};

void sgp30_init_iaq(void);
void sgp30_measure_iaq(struct sgp30_measure_result *measure_out);
void sgp30_measure_raw(struct sgp30_measure_raw_result *measure_out);
void sgp30_set_absolute_humidity(uint8_t abs_hum_gpm3_whole,
				 uint8_t abs_hum_gpm3_fractional);

#endif /* _PICOWEATHER_DRIVERS_SGP30_H */
