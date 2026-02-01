#include "MCP4725.h"
#include "stm32f4xx_hal_def.h"
#include "float.h"

MCP_Result_t MCP_WriteValue(I2C_HandleTypeDef *handle, uint16_t value)
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

uint16_t MCP_VoltageToValue(float voltage)
{
    uint16_t result;

    uint16_t val = Float_To_UInt16(voltage);
    // TODO: converstion math and calibration
    result = (voltage / MCP_Step) - 4095;
    return result;
}