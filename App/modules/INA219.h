#ifndef __INA219_H__
#define __INA219_H__

#include "stm32f4xx_hal.h"

#define INA_I2C_ADDR 0x40 << 1

#define INA_VOLTR 0x02
#define INA_CURR 0x04
#define INA_CALR 0x05

uint8_t INA_Init(I2C_HandleTypeDef *i2c_handle);
uint8_t INA_Read(I2C_HandleTypeDef *i2c_handle, float *voltage, float *current);

#endif