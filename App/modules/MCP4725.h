#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"
#include "stdint.h"
#include <stdint.h>
#include "common.h"

#define MCP_I2C_ADDRESS (0x60 << 1)

#define MCP_STEP_MIN 0 // 15V
#define MCP_STEP_MAX 4095 // 6V
#define MCP_uV_PER_STEP 2242
#define MCP_mV_MAX 15000

typedef struct
{
    _Error_Codes code;
    uint16_t steps;
} MCP_Result_t;

MCP_Result_t MCP_VoltageToSteps(I2C_HandleTypeDef *handle, int32_t target_voltage_whole,
                                uint32_t target_voltage_decimal);
MCP_Result_t MCP_SetSteps(I2C_HandleTypeDef *handle, uint16_t steps);

#endif