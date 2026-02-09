#ifndef __INA219_H__
#define __INA219_H__

#include "stm32f4xx_hal.h"

#define INA_I2C_ADDR 0x40 << 1
#define INA_VOLTR 0x02
#define INA_CURR 0x04
#define INA_CALR 0x05

typedef struct
{
    HAL_StatusTypeDef status;
    float value;
} INA_Result_t;

HAL_StatusTypeDef INA_Init(I2C_HandleTypeDef *handle);

INA_Result_t INA_ReadCurrent(I2C_HandleTypeDef *handle);

INA_Result_t INA_ReadVoltage(I2C_HandleTypeDef *handle);

#endif