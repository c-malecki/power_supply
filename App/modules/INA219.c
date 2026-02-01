#include "INA219.h"

HAL_StatusTypeDef INA_Init(I2C_HandleTypeDef *handle)
{
    uint8_t pData[2] = { 0x50, 0x00 };
    return HAL_I2C_Mem_Write(handle, INA_I2C_ADDR, INA_CALR, 1, pData, 2, 100);
}

// based on equations in ./docs/components/INA219.pdf
// shunt resistor = 0.1Ω
// expected max current = 0.4A
INA_Result_t INA_ReadCurrent(I2C_HandleTypeDef *handle)
{
    INA_Result_t result;
    uint8_t pData[2];

    result.status = HAL_I2C_Mem_Read(handle, INA_I2C_ADDR, INA_CURR, 1, pData, 2, 100);
    if (result.status != HAL_OK)
        return result;

    int16_t raw = (int16_t)((pData[0] << 8) | pData[1]);
    result.value = (float)(raw * 0.02);

    return result;
}

INA_Result_t INA_ReadVoltage(I2C_HandleTypeDef *handle)
{
    INA_Result_t result;
    uint8_t pData[2];

    result.status = HAL_I2C_Mem_Read(handle, INA_I2C_ADDR, 0x02, 1, pData, 2, 100);

    if (result.status != HAL_OK)
        return result;

    uint16_t raw = ((pData[0] << 8) | pData[1]);
    result.value = (raw >> 3) * 0.004f;

    return result;
}