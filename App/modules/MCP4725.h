#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"
#include <sys/types.h>

#define MCP_I2C_ADDR 0x60 << 1

#define MCP_STEP_VOLTAGE 0.002691
#define MCP_STEP_MIN 0
#define MCP_STEP_MAX 3250

uint8_t MCP_SetSteps(I2C_HandleTypeDef *handle, uint16_t steps);
uint16_t MCP_VoltageToSteps(float voltage);
// float MCP_StepsToVoltage(uint16_t steps);

#endif