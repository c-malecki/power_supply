#include "mcp4725.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include "common.h"

uint16_t voltage_to_steps(int32_t target_voltage_whole, uint32_t target_voltage_decimal)
{
    uint32_t pad_dec = target_voltage_decimal;
    if (target_voltage_decimal != 0) {
        if (target_voltage_decimal < 10) {
            pad_dec = target_voltage_decimal * 100;
        }
        if (target_voltage_decimal > 10 && target_voltage_decimal < 100) {
            pad_dec = target_voltage_decimal * 10;
        }
    }

    int32_t mV = (target_voltage_whole * 1000) + pad_dec;
    int32_t steps = ((MCP_mV_MAX - mV) * 1000) / MCP_uV_PER_STEP;

    if (steps < MCP_STEP_MIN) {
        steps = MCP_STEP_MIN;
    }
    if (steps > MCP_STEP_MAX) {
        steps = MCP_STEP_MAX;
    }

    return (uint16_t)steps;
}

MCP_Result_t MCP_VoltageToSteps(I2C_HandleTypeDef *handle, int32_t target_voltage_whole,
                                uint32_t target_voltage_decimal)
{
    MCP_Result_t result;

    uint16_t steps = voltage_to_steps(target_voltage_whole, target_voltage_decimal);

    uint8_t pData[2];

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    result.steps = steps;
    result.code = ConvHALError(HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDRESS, pData, 2, 100));

    return result;
}

MCP_Result_t MCP_SetSteps(I2C_HandleTypeDef *handle, uint16_t steps)
{
    MCP_Result_t result;
    uint8_t pData[2];

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    result.code = ConvHALError(HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDRESS, pData, 2, 100));
    result.steps = steps;

    return result;
}