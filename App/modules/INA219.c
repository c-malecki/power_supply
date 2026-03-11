#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "INA219.h"
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

_Error_Codes INA_Init(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2] = { 0x50, 0x00 };
    return ConvHALError(
        HAL_I2C_Mem_Write(i2c_handle, INA_I2C_ADDRESS, INA_CALIBRATION_REGISTER, 1, pData, 2, 100));
}

INA_Result_t INA_Read_Voltage(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2];
    INA_Result_t result;

    result.code = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDRESS, INA_VOLTAGE_REGISTER, 1, pData, 2, 100));
    if (result.code != ERROR_NONE) {
        return result;
    }

    uint16_t raw_voltage = ((pData[0] << 8) | pData[1]);
    uint32_t millivolts = (raw_voltage >> 3) * 4;

    result.whole = millivolts / 1000;
    result.decimal = millivolts % 1000;

    return result;
}

// based on equations in ./docs/components/INA219.pdf
// shunt resistor = 0.1Ω
// placeholder: estimated max current = 0.4A
INA_Result_t INA_Read_Current(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2];
    INA_Result_t result;

    result.code = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDRESS, INA_CURRENT_REGISTER, 1, pData, 2, 100));
    if (result.code != ERROR_NONE) {
        return result;
    }

    int16_t raw_current = (int16_t)((pData[0] << 8) | pData[1]);
    int32_t microamps = raw_current * 20;

    result.whole = microamps / 1000;
    result.decimal = abs(microamps % 1000);

    return result;
}