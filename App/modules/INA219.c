#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "INA219.h"

static void read_voltage(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result);
static void read_current(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result);

uint8_t INA_Init(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2] = { 0x50, 0x00 };
    return HAL_I2C_Mem_Write(i2c_handle, INA_I2C_ADDRESS, INA_CALIBRATION_REGISTER, 1, pData, 2,
                             100);
}

INA_Read_Result_t INA_Read(I2C_HandleTypeDef *i2c_handle)
{
    INA_Read_Result_t result;

    read_voltage(i2c_handle, &result);
    if (result.error != HAL_OK) {
        return result;
    }

    read_current(i2c_handle, &result);
    if (result.error != HAL_OK) {
        return result;
    }

    result.power = result.voltage * result.current;

    return result;
}

static void read_voltage(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result)
{
    uint8_t pData[2];

    result->error =
        HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDRESS, INA_VOLTAGE_REGISTER, 1, pData, 2, 100);
    if (result->error != HAL_OK) {
        return;
    }

    uint16_t raw_voltage = ((pData[0] << 8) | pData[1]);
    float voltage = (raw_voltage >> 3) * 0.004f;

    result->voltage = voltage;
}

// based on equations in ./docs/components/INA219.pdf
// shunt resistor = 0.1Ω
// placeholder: estimated max current = 0.4A
static void read_current(I2C_HandleTypeDef *i2c_handle, INA_Read_Result_t *result)
{
    uint8_t pData[2];

    result->error =
        HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDRESS, INA_CURRENT_REGISTER, 1, pData, 2, 100);
    if (result->error != HAL_OK) {
        return;
    }

    int16_t raw_current = (int16_t)((pData[0] << 8) | pData[1]);
    float milliamps = raw_current * 0.02;

    result->current = milliamps;
}