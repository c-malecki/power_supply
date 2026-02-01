#include "MCP4725.h"

HAL_StatusTypeDef MCP_Write_Value(I2C_HandleTypeDef *handle, uint16_t value)
{
    uint8_t pData[2];

    if (value < DAC_VAL_12V)
        value = DAC_VAL_12V;

    if (value > DAC_VAL_3V3)
        value = DAC_VAL_3V3;

    pData[0] = (value >> 8) & 0x0F;
    pData[1] = value & 0xFF;

    return HAL_I2C_Master_Transmit(handle, MCP_I2C_ADDR, pData, 2, 100);
}