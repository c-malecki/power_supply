#ifndef __POWER_CONTROLLER_H__
#define __POWER_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "MCP4725.h"

#define CHANNEL_VAR_VOLTAGE_TOLLERANCE 0.05f

// typedef enum {
//     CHAN_3V3 = 0,
//     CHAN_5V,
//     CHAN_VAR,
//     CHAN_CT
// } PWR_Channels;

typedef struct
{
    float target_voltage;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
} Channel_InitStruct;

typedef struct
{
    float target_voltage;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
    bool output_enabled;
} Channel_VDC_t;

typedef struct
{
    float p_gain;
    float i_gain;
    float acc_err;
    float prev_error;
    uint32_t last_time;
} Channel_VAR_PID_t;

typedef enum {
    ROTARY_MODE_OFF = 0,
    ROTARY_MODE_ADJUST,
    ROTARY_MODE_CONFIRM,
} Channel_VAR_Rotary_Modes_t;

typedef struct
{
    GPIO_TypeDef *clk_port;
    GPIO_TypeDef *dt_port;
    GPIO_TypeDef *sw_port;
    uint16_t clk_pin;
    uint16_t dt_pin;
    uint16_t sw_pin;
    uint8_t last_clk;
    int32_t position;
    bool pressed;
    Channel_VAR_Rotary_Modes_t mode;
} Channel_VAR_Rotary_t;

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

typedef struct
{
    float target_voltage;
    GPIO_TypeDef *mosfet_port;
    uint16_t mosfet_pin;
    bool output_enabled;
    uint16_t cur_dac_steps;
    float cur_voltage;
    float cur_current;
    float cur_power;
    Channel_VAR_Adjustment_State_t adjustment_state;
    Channel_VAR_PID_t pid;
    Channel_VAR_Rotary_t rotary;
} Channel_VAR_t;

typedef struct
{
    Channel_VDC_t *chan_3v3; // controlled by buck and MOSFET only
    Channel_VDC_t *chan_5v; // controlled by buck and MOSFET only
    Channel_VAR_t *chan_var; // controlled by buck, dac, current sensor, and PI loop
} Power_Controller_t;

uint8_t Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
uint8_t Power_Controller_StartupTest(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);

void Channel_VDC_EnableOutput(Channel_VDC_t *chan, bool enabled);
void Channel_VAR_EnableOutput(Channel_VAR_t *chan, bool enabled);

uint8_t Channel_VAR_UpdateValues(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle);
uint8_t Channel_VAR_PollRotary(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle);
uint8_t Channel_VAR_SetVoltage(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle,
                               float target_voltage);

#endif // __POWER_CONTROLLER_H__