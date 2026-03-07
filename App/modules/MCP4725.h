#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include <stdint.h>

#define MCP_I2C_ADDRESS 0x60 << 1

#define MCP_STEP_VOLTAGE 0.002704f

uint16_t MCP_VoltageToSteps(float target_voltage);
uint8_t MCP_SetSteps(I2C_HandleTypeDef *handle, uint16_t steps);

#endif