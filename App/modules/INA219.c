#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "INA219.h"

static uint8_t read_voltage(I2C_HandleTypeDef *i2c_handle, float *voltage);
static uint8_t read_current(I2C_HandleTypeDef *i2c_handle, float *current);

uint8_t INA_Init(I2C_HandleTypeDef *i2c_handle)
{
    uint8_t pData[2] = { 0x50, 0x00 };
    return HAL_I2C_Mem_Write(i2c_handle, INA_I2C_ADDR, INA_CALR, 1, pData, 2, 100);
}

uint8_t INA_Read(I2C_HandleTypeDef *i2c_handle, float *voltage, float *current)
{
    uint8_t err = read_voltage(i2c_handle, voltage);
    if (err != HAL_OK) {
        return err;
    }

    err = read_current(i2c_handle, current);
    if (err != HAL_OK) {
        return err;
    }

    return err;
}

static uint8_t read_voltage(I2C_HandleTypeDef *i2c_handle, float *voltage)
{
    uint8_t err;
    uint8_t pData[2];

    err = HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDR, INA_VOLTR, 1, pData, 2, 100);

    if (err != HAL_OK) {
        return err;
    }

    uint16_t raw = ((pData[0] << 8) | pData[1]);
    float v = (raw >> 3) * 0.004f;

    *voltage = v;

    return err;
}

// based on equations in ./docs/components/INA219.pdf
// shunt resistor = 0.1Ω
// placeholder: estimated max current = 0.4A
static uint8_t read_current(I2C_HandleTypeDef *i2c_handle, float *current)
{
    uint8_t err;
    uint8_t pData[2];

    err = HAL_I2C_Mem_Read(i2c_handle, INA_I2C_ADDR, INA_CURR, 1, pData, 2, 100);
    if (err != HAL_OK) {
        return err;
    }

    int16_t raw = (int16_t)((pData[0] << 8) | pData[1]);
    float mA = raw * 0.02;

    *current = mA;

    return err;
}