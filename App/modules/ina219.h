#ifndef __INA219_H__
#define __INA219_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

#define INA_I2C_ADDR_MAIN (0x40 << 1)
#define INA_I2C_ADDR_VAR (0x41 << 1)
#define INA_I2C_ADDR_6V5 (0x44 << 1)
#define INA_I2C_ADDR_12V (0x45 << 1)

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
    uint32_t i2c_addr;
    uint32_t last_read;
    int32_t voltage_w;
    uint32_t voltage_d;
    int32_t current_w;
    uint32_t current_d;
} INA_t;

_Error_Codes INA_Init(INA_t *ina, I2C_HandleTypeDef *i2c_handle);
_Error_Codes INA_Read(INA_t *ina, I2C_HandleTypeDef *i2c_handle);

#endif