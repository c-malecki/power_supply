#include "MCP4725.h"
#include "stm32f4xx_hal_def.h"
#include "float.h"
#include "stdint.h"
#include <stdint.h>

MCP_Result_t MCP_WriteSteps(I2C_HandleTypeDef *handle, uint16_t value)
{
    MCP_Result_t result;
    result.value = value;

    uint8_t pData[2];

    if (value < MCP_VAL_12V)
        value = MCP_VAL_12V;

    if (value > MCP_VAL_3V3)
        value = MCP_VAL_3V3;

    pData[0] = (value >> 8) & 0x0F;
    pData[1] = value & 0xFF;

    result.status = HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDR, pData, 2, 100);
    if (result.status != HAL_OK)
        return result;

    return result;
}

uint16_t MCP_VoltageToSteps(float voltage)
{
    return (uint16_t)Float_To_UInt16((12 - voltage) / MCP_Step);
}