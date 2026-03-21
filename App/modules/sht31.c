#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include "common.h"
#include "sht31.h"

_Error_Codes SHT_Init(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t cmd[2] = { SHT_CMD_SOFT_RESET_MSB, SHT_CMD_SOFT_RESET_LSB };
    _Error_Codes code =
        ConvHALError(HAL_I2C_Master_Transmit(i2c_handle, SHT_I2C_ADDRESS, cmd, 2, 100));
    HAL_Delay(2); // reset takes ~1.5ms
    return code;
}

SHT_Result_t SHT_Read_Temperature(I2C_HandleTypeDef *i2c_handle)
{
    SHT_Result_t result = { 0 };
    uint8_t cmd[2] = { SHT_CMD_MEASURE_MSB, SHT_CMD_MEASURE_LSB };

    result.code = ConvHALError(HAL_I2C_Master_Transmit(i2c_handle, SHT_I2C_ADDRESS, cmd, 2, 100));
    if (result.code != ERROR_NONE) {
        return result;
    }

    // TODO: remove delay and use some sort of timer
    HAL_Delay(15);

    uint8_t data[6];
    result.code = ConvHALError(HAL_I2C_Master_Receive(i2c_handle, SHT_I2C_ADDRESS, data, 6, 100));
    if (result.code != ERROR_NONE) {
        return result;
    }

    uint16_t raw = ((data[0] << 8) | data[1]);

    result.temp_c = -45 + (175 * (int32_t)raw / 65535);

    return result;
}