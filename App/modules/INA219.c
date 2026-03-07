#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "INA219.h"
#include <stdio.h>
#include "common.h"

static void read_voltage(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result);
static void read_current(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result);

_Error_Codes INA_Init(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2] = { 0x50, 0x00 };
    return ConvHALError(
        HAL_I2C_Mem_Write(i2c_handle, INA_I2C_ADDRESS, INA_CALIBRATION_REGISTER, 1, pData, 2, 100));
}

INA_Read_Result_t INA_Read(I2C_HandleTypeDef *i2c_handle)
{
    INA_Read_Result_t result;

    read_voltage(i2c_handle, &result);
    if (result.error != ERROR_NONE) {
        return result;
    }

    read_current(i2c_handle, &result);
    if (result.error != ERROR_NONE) {
        return result;
    }

    result.power = result.voltage * result.current;

    return result;
}

static void read_voltage(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result)
{
    uint8_t pData[2];

    result->error = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDRESS, INA_VOLTAGE_REGISTER, 1, pData, 2, 100));
    if (result->error != ERROR_NONE) {
        return;
    }

    uint16_t raw_voltage = ((pData[0] << 8) | pData[1]);
    printf("raw_voltage: %u\r\n", raw_voltage);
    float voltage = (raw_voltage >> 3) * 0.004f;

    result->voltage = voltage;
}

// based on equations in ./docs/components/INA219.pdf
// shunt resistor = 0.1Ω
// placeholder: estimated max current = 0.4A
static void read_current(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result)
{
    uint8_t pData[2];

    result->error = ConvHALError(
        HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDRESS, INA_CURRENT_REGISTER, 1, pData, 2, 100));
    if (result->error != ERROR_NONE) {
        return;
    }

    int16_t raw_current = (int16_t)((pData[0] << 8) | pData[1]);
    printf("raw_current: %u\r\n", raw_current);
    float milliamps = raw_current * 0.02;

    result->current = milliamps;
}