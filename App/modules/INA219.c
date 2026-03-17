#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "INA219.h"
#include <stdlib.h>
#include "common.h"

_Error_Codes INA_Init(uint32_t ina_addr, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2] = { INA_CALIBRATION_VALUE_0, INA_CALIBRATION_VALUE_1 };
    return ConvHALError(
        HAL_I2C_Mem_Write(i2c_handle, ina_addr, INA_CALIBRATION_REGISTER, 1, pData, 2, 100));
}

INA_Result_t INA_Read_Voltage(uint32_t ina_addr, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2];
    INA_Result_t result;

    result.code = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, ina_addr, INA_VOLTAGE_REGISTER, 1, pData, 2, 100));
    if (result.code != ERROR_NONE) {
        return result;
    }

    uint16_t raw = ((pData[0] << 8) | pData[1]);
    uint32_t millivolts = (raw >> 3) * 4;

    result.whole = millivolts / 1000;
    result.decimal = millivolts % 1000;

    return result;
}

// TODO: fix multimeter or get new one that will read current to verify INA calibration
INA_Result_t INA_Read_Current(uint32_t ina_addr, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2];
    INA_Result_t result;

    // per research, a sharp load can reset the INA219 which will reset the calibrartion register
    // so this is extraoverhead but to guarentee correct functionality and readings
    result.code = INA_Init(ina_addr, i2c_handle);
    if (result.code != ERROR_NONE) {
        return result;
    }

    result.code = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, ina_addr, INA_CURRENT_REGISTER, 1, pData, 2, 100));
    if (result.code != ERROR_NONE) {
        return result;
    }

    int16_t raw = (int16_t)((pData[0] << 8) | pData[1]);
    int32_t nanoamps = ((int32_t)raw * INA_CURRENT_MULTIPLIER_nA);

    result.whole = nanoamps / 1000000;
    result.decimal = abs((nanoamps % 1000000) / 1000);

    return result;
}