#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef struct
{
    // cfg
    I2C_HandleTypeDef *i2c_handle;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
    // state
    uint16_t mcp_target_voltage;
    float cur_voltage;
    float cur_current;
    bool output_enabled;
} Power_Controller_t;

typedef struct
{
    float voltage;
    float current;
} Power_Read_Result_t;

typedef enum {
    PWR_OK = 0,
    PWR_ERROR_I2C,
    PWR_ERROR_OVERCURRENT,
    PWR_ERROR_OVERVOLTAGE
} Power_Status_t;

Power_Status_t Power_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c,
                          GPIO_TypeDef *mosfet_port, uint16_t mosfet_pin);
Power_Status_t Power_SetVoltage(Power_Controller_t *ctrl, uint16_t value);
Power_Status_t Power_Enable(Power_Controller_t *ctrl, bool enable);
Power_Status_t Power_Update(Power_Controller_t *ctrl);
Power_Read_Result_t Power_Read(Power_Controller_t *ctrl);
void Power_SweepRange(Power_Controller_t *ctrl, uint8_t *count);

#endif