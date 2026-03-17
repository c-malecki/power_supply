#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "MCP4725.h"
#include "common.h"

#define VOLTAGE_VARIABLE_MIN_WHOLE 6
#define VOLTAGE_VARIABLE_MIN_DECIMAL 0

#define VOLTAGE_VARIABLE_MAX_WHOLE 15
#define VOLTAGE_VARIABLE_MAX_DECIMAL 0

#define VARIABLE_VOLTAGE_TOLLERANCE 0.05f

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

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
            Channel_VAR_PID_t pid;
        } variable;
    };
} Power_Controller_Channel_t;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Power_Controller_Channel_t channels[3];
    int32_t main_voltage_whole;
    uint32_t main_voltage_decimal;
    int32_t main_current_whole;
    uint32_t main_current_decimal;
    Error_Callback_t error_cb;
    void *error_ctx;
} Power_Controller_t;

void Power_Controller_PingMainINA(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Power_Controller_PingVarINA(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Power_Controller_PingMCP(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);

void Power_Controller_UpdateMainValues(Power_Controller_t *ctrl);

void Power_Controller_EnableOut(Power_Controller_t *ctrl, Power_Channels chan, bool enabled);
void Power_Controller_SetVoltage(Power_Controller_t *ctrl, int32_t target_voltage_whole,
                                 uint32_t target_voltage_decimal);
void Power_Controller_UpdateVarValues(Power_Controller_t *ctrl);

void Power_Controller_PrintState(Power_Controller_t *ctrl);

#endif // __POWER_CONTROLLER_H__