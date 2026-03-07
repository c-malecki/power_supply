#ifndef __INA219_H__
#define __INA219_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define INA_I2C_ADDRESS 0x40 << 1

#define INA_VOLTAGE_REGISTER 0x02
#define INA_CURRENT_REGISTER 0x04
#define INA_CALIBRATION_REGISTER 0x05
#define INA_CURRENT_TRESHOLD 0.5f

typedef struct
{
    uint8_t error;
    float voltage;
    float current;
    float power;
} INA_Read_Result_t;

uint8_t INA_Init(I2C_HandleTypeDef *i2c_handle);
INA_Read_Result_t INA_Read(I2C_HandleTypeDef *i2c_handle);

#endif