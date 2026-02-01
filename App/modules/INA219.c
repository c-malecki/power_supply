#include "INA219.h"

HAL_StatusTypeDef INA_Init(I2C_HandleTypeDef *handle)
{
    uint8_t pData[2] = { 0x50, 0x00 };
    return HAL_I2C_Mem_Write(handle, INA_I2C_ADDR, INA_CALR, 1, pData, 2, 100);
}

// based on equations in ./docs/components/INA219.pdf
// shunt resistor = 0.1Ω
// expected max current = 0.4A
double INA_Read_Current(I2C_HandleTypeDef *handle)
{
    uint8_t pData[2];

    HAL_StatusTypeDef err = HAL_I2C_Mem_Read(handle, INA_I2C_ADDR, INA_CURR, 1, pData, 2, 100);
    if (err != HAL_OK) {
        return 0.0;
    }

    int16_t raw_current = (int16_t)((pData[0] << 8) | pData[1]);

    return (double)(raw_current * 0.02);
}

double INA_Read_Voltage(I2C_HandleTypeDef *handle)
{
    uint8_t pData[2];

    HAL_StatusTypeDef err = HAL_I2C_Mem_Read(handle, INA_I2C_ADDR, 0x02, 1, pData, 2, 100);

    if (err != HAL_OK)
        return 0.0f;

    uint16_t ret = ((pData[0] << 8) | pData[1]);

    return (ret >> 3) * 0.004f;
    ;
}