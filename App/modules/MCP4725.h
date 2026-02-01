#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"

#define MCP_I2C_ADDR 0x60 << 1

#define DAC_VAL_3V3 3236
#define DAC_VAL_5V 2560
#define DAC_VAL_9V 1133
#define DAC_VAL_12V 0

HAL_StatusTypeDef MCP_Write_Value(I2C_HandleTypeDef *handle, uint16_t value);

#endif