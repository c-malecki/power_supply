#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "MCP4725.h"
#include "common.h"

#define VOLTAGE_VARIABLE_MIN_WHOLE 2
#define VOLTAGE_VARIABLE_MIN_DECIMAL 0

#define VOLTAGE_VARIABLE_MAX_WHOLE 12
#define VOLTAGE_VARIABLE_MAX_DECIMAL 0

#define VARIABLE_VOLTAGE_TOLLERANCE 0.05f

#define VOLTAGE_3V3_WHOLE 3
#define VOLTAGE_3V3_DECIMAL 3

#define VOLTAGE_5V_WHOLE 5
#define VOLTAGE_5V_DECIMAL 0

#define VOLTAGE_9V_WHOLE 9
#define VOLTAGE_9V_DECIMAL 0

#define VOLTAGE_10V_WHOLE 10
#define VOLTAGE_10V_DECIMAL 0

typedef enum {
    POWER_CHANNEL_3V3 = 0,
    POWER_CHANNEL_5V,
    POWER_CHANNEL_VARIABLE
} Power_Channels;

typedef struct
{
    float p_gain;
    float i_gain;
    float acc_error;
    float prev_error;
    uint32_t last_time;
} Channel_VAR_PID_t;

typedef enum {
    IDLE,
    SETTLING,
    STABLE
} Channel_VAR_Adjustments_States_t;

typedef struct
{
    Channel_VAR_Adjustments_States_t state;
    uint32_t start_time;
    uint16_t delay_ms;
} Channel_VAR_Adjustment_State_t;

typedef enum {
    CHANNEL_TYPE_FIXED = 0,
    CHANNEL_TYPE_VARIABLE,
} Power_Channel_Types;

typedef struct
{
    bool output_enabled;
    Power_Channel_Types channel_type;
    uint16_t mosfet_pin;
    int32_t target_voltage_whole;
    uint32_t target_voltage_decimal;
    GPIO_TypeDef *mosfet_port;

    union {
        struct
        {
            Channel_VAR_Adjustment_State_t adjustment_state;
            uint16_t cur_dac_steps;
            int32_t cur_voltage_whole;
            uint32_t cur_voltage_decimal;
            int32_t cur_current_whole;
            uint32_t cur_current_decimal;
            // float cur_power;
            Channel_VAR_PID_t pid;
        } variable;
    };
} Power_Controller_Channel_t;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Power_Controller_Channel_t channels[3];
} Power_Controller_t;

typedef struct
{
    _Error_Codes mcp;
    _Error_Codes ina;
} Power_Controller_Ping_Result_t;

typedef struct
{
    _Peripherals peripheral;
    _Error_Codes error;
} Power_Controller_SetVariableVoltage_Result_t;

Power_Controller_Ping_Result_t Power_Controller_PingPeripherals(I2C_HandleTypeDef *i2c_handle);
_Error_Codes Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Power_Controller_EnableChannel(Power_Controller_t *ctrl, Power_Channels chan, bool enabled);
Power_Controller_SetVariableVoltage_Result_t
Power_Controller_SetVariableVoltage(Power_Controller_t *ctrl, int32_t target_voltage_whole,
                                    uint32_t target_voltage_decimal);
void Power_Controller_PrintState(Power_Controller_t *ctrl);

#endif // __POWER_CONTROLLER_H__