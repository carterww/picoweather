#ifndef _PICOWEATHER_DRIVERS_BME280_H
#define _PICOWEATHER_DRIVERS_BME280_H

/* The BME280 is a humidity, pressure, and temperature sensor that
 * communicates over I2C or SPI. The humidity and pressure sensor can
 * be independently enabled/disabled and it has a sleep mode.
 *
 * The temperature sensor is mostly used for temperature compensation of
 * the pressure and humidity sensors. It can be used to estimate ambient
 * temperatures.
 *
 * I2C specs:
 *
 * Random things:
 * - The sensor has a sleep mode that only consumes 1uA.
 * - Measurements can set to be performed in regular intervals.
 * - Keeping an interface pin high while V_{DDIO} is low can permanently
 *   damage the device.
 *
 * Datasheet: https://cdn-learn.adafruit.com/assets/assets/000/115/588/original/bst-bme280-ds002.pdf?1664822559
 */

/* When SDO is connected to GND, 0x77 when connected to V_{DDIO} */
#define BME280_I2C_ADDRESS ((uint8_t)0x76)

enum bme280_mode {
	bme280_mode_sleep,
	bme280_mode_normal,
	bme280_mode_forced,
};

void bme280_init(void);

/* Transition the BME280 to a new mode. Here is a short description
 * of each mode:
 * Sleep Mode: Initial mode where the device doesn't operate. Registers
 * are still accessible.
 * Normal Mode: Device measures periodically where the time between
 * measurements can be calibrated.
 * Forced Mode: Device measures once then goes back to sleep. The results
 * are still accessible via the registers.
 */
int bme280_mode_transition(enum bme280_mode mode);

#endif /* _PICOWEATHER_DRIVERS_BME280_H */
