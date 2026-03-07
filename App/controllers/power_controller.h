#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "MCP4725.h"
#include "common.h"

#define VOLTAGE_VARIABLE_MIN 2.0f
#define VOLTAGE_VARIABLE_MAX 12.0f
#define VARIABLE_VOLTAGE_TOLLERANCE 0.05f

#define VOLTAGE_3V3 3.3f
#define VOLTAGE_5V 5.0f
#define VOLTAGE_9V 9.0f
#define VOLTAGE_10V 10.0f

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
    float target_voltage;
    GPIO_TypeDef *mosfet_port;

    union {
        struct
        {
            Channel_VAR_Adjustment_State_t adjustment_state;
            uint16_t cur_dac_steps;
            float cur_voltage;
            float cur_current;
            float cur_power;
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
Power_Controller_SetVariableVoltage(Power_Controller_t *ctrl, float target_voltage);
void Power_Controller_PrintState(Power_Controller_t *ctrl);

#endif // __POWER_CONTROLLER_H__