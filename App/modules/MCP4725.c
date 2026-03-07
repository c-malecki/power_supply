#include "MCP4725.h"
#include "stm32f4xx_hal_def.h"
#include "stdint.h"
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include "common.h"

uint16_t float_to_uint16(float value)
{
    if (!isfinite(value)) {
        return 0;
    }

    float rounded_f = round(value);

    if (rounded_f < 0.0f) {
        return 0;
    } else if (rounded_f > (float)UINT16_MAX) {
        return UINT16_MAX;
    }

    return (uint16_t)rounded_f;
}

uint16_t voltage_to_steps(int32_t target_voltage_whole, uint32_t target_voltage_decimal)
{
    int32_t mV = (target_voltage_whole * 1000) + target_voltage_decimal;
    int32_t uV = (MCP_MAX_mV - mV) * 1000;
    return (uint16_t)(uV / MCP_STEP_uV);
}

MCP_SetSteps_Result_t MCP_SetSteps(I2C_HandleTypeDef *handle, int32_t target_voltage_whole,
                                   uint32_t target_voltage_decimal)
{
    MCP_SetSteps_Result_t result;

    uint16_t steps = voltage_to_steps(target_voltage_whole, target_voltage_decimal);

    uint8_t pData[2];

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    result.steps = steps;
    result.error = ConvHALError(HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDRESS, pData, 2, 100));

    return result;
}