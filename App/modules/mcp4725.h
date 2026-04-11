#ifndef __MCP4725_H__
#define __MCP4725_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

#define MCP_I2C_ADDR (0x60 << 1)

#define MCP_STEP_MIN 0 // 15V
#define MCP_STEP_MAX 4095 // 6V
#define MCP_uV_PER_STEP 2242
#define MCP_mV_MAX 15000

typedef struct
{
    uint32_t i2c_addr;
    uint16_t steps;
} MCP_t;

_Error_Codes MCP_VoltageToSteps(MCP_t *mcp, I2C_HandleTypeDef *handle, int32_t target_voltage_w,
                                uint32_t target_voltage_d);
_Error_Codes MCP_SetSteps(MCP_t *mcp, I2C_HandleTypeDef *handle, uint16_t steps);

#endif