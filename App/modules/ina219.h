#ifndef __INA219_H__
#define __INA219_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

#define INA_I2C_ADDRESS_MAIN (0x40 << 1)
#define INA_I2C_ADDRESS_VAR (0x41 << 1)
#define INA_I2C_ADDRESS_6V5 (0x44 << 1)
#define INA_I2C_ADDRESS_12V (0x45 << 1)

#define INA_VOLTAGE_REGISTER (0x02)
#define INA_CURRENT_REGISTER (0x04)
#define INA_CALIBRATION_REGISTER (0x05)
// based on equations in ./docs/components/INA219.pdf
// shunt resistor = 0.1Ω
// placeholder: estimated max current = 0.4A
// Current_LSB: 0.4 / 2^15 = 0.000012207
// Calibrartion value: 0.04096 / (0.000012207 * 0.1) = 33554
#define INA_CALIBRATION_VALUE_0 (0x83)
#define INA_CALIBRATION_VALUE_1 (0x12)
// Current_LSB scaled to nA for non-floating point
#define INA_CURRENT_MULTIPLIER_nA 12207
#define INA_CURRENT_TRESHOLD 0.5f

typedef struct
{
    _Error_Codes code;
    int32_t whole;
    uint32_t decimal;
} INA_Result_t;

_Error_Codes INA_Init(uint32_t ina_addr, I2C_HandleTypeDef *i2c_handle);
INA_Result_t INA_Read_Voltage(uint32_t ina_addr, I2C_HandleTypeDef *i2c_handle);
INA_Result_t INA_Read_Current(uint32_t ina_addr, I2C_HandleTypeDef *i2c_handle);

#endif