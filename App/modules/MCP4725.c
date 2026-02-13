#include "MCP4725.h"
#include "stm32f4xx_hal_def.h"
#include "float.h"
#include "stdint.h"
#include <stdint.h>

uint8_t MCP_SetSteps(I2C_HandleTypeDef *handle, uint16_t steps)
{
    uint8_t pData[2];

    if (steps < MCP_VAL_12V)
        steps = MCP_VAL_12V;

    if (steps > MCP_VAL_3V3)
        steps = MCP_VAL_3V3;

    pData[0] = (steps >> 8) & 0x0F;
    pData[1] = steps & 0xFF;

    return HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDR, pData, 2, 100);
}

uint16_t MCP_VoltageToSteps(float voltage)
{
    return (uint16_t)Float_To_UInt16((12 - voltage) / MCP_Step);
}

float MCP_StepsToVoltage(uint16_t steps)
{
    return steps * MCP_Step;
}