#ifndef _PICOWEATHER_CFG_H
#define _PICOWEATHER_CFG_H

#include <hardware/i2c.h>

#define BUILD_RELEASE (0)
#define BUILD_DEBUG (1)
#define BUILD_RELDEBUG (2)

#define BUILD_TYPE BUILD_DEBUG

#define ADC_NDEVICES (1)
#define ADC_VREF_MV (3300)
#define ADC_READ_MAX (4095)
#define ADC0_GPIO_PIN (26U)
#define ADC1_GPIO_PIN (27U)
#define ADC2_GPIO_PIN (28U)

#define I2C_STANDARD_MODE_HZ (100000)

#define S12SD_GPIO_PIN ADC2_GPIO_PIN

#define BH1750_I2C_INST_N (0)
#define BH1750_I2C_INST (i2c0_inst)
#define BH1750_GPIO_PIN_I2C_SDA (12U)
#define BH1750_GPIO_PIN_I2C_SCL (13U)

#endif /* _PICOWEATHER_CFG_H */
