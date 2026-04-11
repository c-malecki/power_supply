#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "ina219.h"
#include <stdlib.h>
#include "common.h"

_Error_Codes INA_Init(INA_t *ina, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2] = { INA_CALIBRATION_VALUE_0, INA_CALIBRATION_VALUE_1 };
    return ConvHALError(
        HAL_I2C_Mem_Write(i2c_handle, ina->i2c_addr, INA_CALIBRATION_REGISTER, 1, pData, 2, 100));
}

_Error_Codes INA_Read(INA_t *ina, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t vData[2];

    _Error_Codes code = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, ina->i2c_addr, INA_VOLTAGE_REGISTER, 1, vData, 2, 100));
    if (code != ERROR_NONE) {
        return code;
    }

    uint16_t rawv = ((vData[0] << 8) | vData[1]);
    uint32_t millivolts = (rawv >> 3) * 4;

    ina->voltage_w = millivolts / 1000;
    ina->voltage_d = millivolts % 1000;

    // TODO: fix multimeter or get new one that will read current to verify INA calibration
    uint8_t cData[2];

    // per research, a sharp load can reset the INA219 which will reset the calibrartion register
    // so this is extraoverhead but to guarentee correct functionality and readings
    code = INA_Init(ina, i2c_handle);
    if (code != ERROR_NONE) {
        return code;
    }

    code = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, ina->i2c_addr, INA_CURRENT_REGISTER, 1, cData, 2, 100));
    if (code != ERROR_NONE) {
        return code;
    }

    int16_t rawc = (int16_t)((cData[0] << 8) | cData[1]);
    int32_t nanoamps = ((int32_t)rawc * INA_CURRENT_MULTIPLIER_nA);

    ina->current_w = nanoamps / 1000000;
    ina->current_d = abs((nanoamps % 1000000) / 1000);

    return ERROR_NONE;
}