#include "MCP4725.h"
#include "stm32f4xx_hal_def.h"
#include "float.h"
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

uint16_t voltage_to_steps(float target_voltage)
{
    return (uint16_t)float_to_uint16((12.0f - target_voltage) / MCP_STEP_VOLTAGE);
}

MCP_SetSteps_Result_t MCP_SetSteps(I2C_HandleTypeDef *handle, float target_voltage)
{
    MCP_SetSteps_Result_t result;

    uint16_t steps = voltage_to_steps(target_voltage);

    uint8_t pData[2];

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    result.steps = steps;
    result.error = ConvHALError(HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDRESS, pData, 2, 100));

    return result;
}

// float steps_to_voltage(uint16_t steps)
// {
//     return steps * MCP_STEP_VOLTAGE;
// }