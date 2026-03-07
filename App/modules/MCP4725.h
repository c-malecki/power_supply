#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include <stdint.h>
#include "common.h"

#define MCP_I2C_ADDRESS 0x60 << 1

#define MCP_STEP_uV 2704
#define MCP_MAX_mV 12000

typedef struct
{
    _Error_Codes error;
    uint16_t steps;
} MCP_SetSteps_Result_t;

MCP_SetSteps_Result_t MCP_SetSteps(I2C_HandleTypeDef *handle, int32_t target_voltage_whole,
                                   uint32_t target_voltage_decimal);

#endif