#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"

#define MCP_I2C_ADDR 0x60 << 1

#define MCP_VAL_12V 0
#define MCP_VAL_9V 1130
#define MCP_VAL_5V 2590
#define MCP_VAL_3V3 3232

#define MCP_Step 0.00269

typedef struct
{
  HAL_StatusTypeDef status;
  uint16_t value;
} MCP_Result_t;

MCP_Result_t MCP_SetSteps(I2C_HandleTypeDef *handle, uint16_t value);

uint16_t MCP_VoltageToSteps(float voltage);

float MCP_StepsToVoltage(uint16_t steps);

#endif