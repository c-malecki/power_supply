#include "MCP4725.h"
#include "stm32f4xx_hal_def.h"
#include "float.h"
#include "stdint.h"
#include <stdint.h>

uint16_t MCP_VoltageToSteps(float target_voltage)
{
    if (target_voltage < MCP_MIN_VOLTAGE)
        target_voltage = MCP_MIN_VOLTAGE;

    if (target_voltage > MCP_MAX_VOLTAGE)
        target_voltage = MCP_MAX_VOLTAGE;

    return (uint16_t)Float_To_UInt16((12.0f - target_voltage) / MCP_STEP_VOLTAGE);
}

// float steps_to_voltage(uint16_t steps)
// {
//     return steps * MCP_STEP_VOLTAGE;
// }

uint8_t MCP_SetSteps(I2C_HandleTypeDef *handle, uint16_t steps)
{
    uint8_t pData[2];

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    uint8_t err = HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDR, pData, 2, 100);
    if (err != HAL_OK)
        return err;

    return HAL_OK;
}