#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"

#define MCP_I2C_ADDR 0x60 << 1

#define MCP_VAL_3V3 3236
#define MCP_VAL_5V 2560
#define MCP_VAL_9V 1133
#define MCP_VAL_12V 0

// TODO: Recheck values and calibration
#define MCP_Step 0.002595

typedef struct
{
    HAL_StatusTypeDef status;
    uint16_t value;
} MCP_Result_t;

MCP_Result_t MCP_WriteValue(I2C_HandleTypeDef *handle, uint16_t value);
uint16_t MCP_VoltageToValue(float voltage);

#endif