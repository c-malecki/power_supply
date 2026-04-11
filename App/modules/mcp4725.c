#include "mcp4725.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include "common.h"

uint16_t voltage_to_steps(int32_t target_voltage_w, uint32_t target_voltage_d)
{
    uint32_t pad_dec = target_voltage_d;
    if (target_voltage_d != 0) {
        if (target_voltage_d < 10) {
            pad_dec = target_voltage_d * 100;
        }
        if (target_voltage_d > 10 && target_voltage_d < 100) {
            pad_dec = target_voltage_d * 10;
        }
    }

    int32_t mV = (target_voltage_w * 1000) + pad_dec;
    int32_t steps = ((MCP_mV_MAX - mV) * 1000) / MCP_uV_PER_STEP;

    if (steps < MCP_STEP_MIN) {
        steps = MCP_STEP_MIN;
    }
    if (steps > MCP_STEP_MAX) {
        steps = MCP_STEP_MAX;
    }

    return (uint16_t)steps;
}

_Error_Codes MCP_VoltageToSteps(MCP_t *mcp, I2C_HandleTypeDef *handle, int32_t target_voltage_w,
                                uint32_t target_voltage_d)
{
    _Error_Codes code;

    uint16_t steps = voltage_to_steps(target_voltage_w, target_voltage_d);

    uint8_t pData[2];

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    mcp->steps = steps;
    code = ConvHALError(HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDR, pData, 2, 100));

    return code;
}

_Error_Codes MCP_SetSteps(MCP_t *mcp, I2C_HandleTypeDef *handle, uint16_t steps)
{
    _Error_Codes code;
    uint8_t pData[2];

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    code = ConvHALError(HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDR, pData, 2, 100));
    mcp->steps = steps;

    return code;
}