#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "gpio.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
//
#include "INA219.h"
#include "MCP4725.h"
#include "common.h"
#include "power_controller.h"

static const Power_Controller_Channel_t channel_3v3_default = {
    .channel_type = CHANNEL_TYPE_FIXED,
    .mosfet_pin = MOSFET_CHAN_3V3_Pin,
    .mosfet_port = MOSFET_CHAN_3V3_GPIO_Port,
    .output_enabled = false,
    .target_voltage_whole = VOLTAGE_3V3_WHOLE,
    .target_voltage_decimal = VOLTAGE_3V3_DECIMAL
};

static const Power_Controller_Channel_t channel_5v_default = {
    .channel_type = CHANNEL_TYPE_FIXED,
    .mosfet_pin = MOSFET_CHAN_5V_Pin,
    .mosfet_port = MOSFET_CHAN_5V_GPIO_Port,
    .output_enabled = false,
    .target_voltage_whole = VOLTAGE_5V_WHOLE,
    .target_voltage_decimal = VOLTAGE_5V_DECIMAL
};

static const Power_Controller_Channel_t channel_var_default = {
    .output_enabled         = false,
    .channel_type           = CHANNEL_TYPE_VARIABLE,
    .mosfet_pin             = MOSFET_CHAN_VAR_Pin,
    .target_voltage_whole   = VOLTAGE_VARIABLE_MIN_WHOLE,
    .target_voltage_decimal = VOLTAGE_VARIABLE_MIN_DECIMAL,
    .mosfet_port            = MOSFET_CHAN_VAR_GPIO_Port,
    .variable               = {
                      .adjustment_state    = {0},
                      .cur_dac_steps       = 0,
                      .cur_voltage_whole   = 0,
                      .cur_voltage_decimal = 0,
                      .cur_current_whole   = 0,
                      .cur_current_decimal = 0,
                      .pid =
            {
                              .p_gain     = 0.0f,
                              .i_gain     = 0.0f,
                              .acc_error  = 0.0f,
                              .prev_error = 0.0f,
                              .last_time  = 0,
            },
    }};

void Power_Controller_PingINA(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    _Error_Codes code = ConvHALError(HAL_I2C_IsDeviceReady(i2c_handle, INA_I2C_ADDRESS, 3, 100));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_INA,
                                    .code = code,
                                    .function = FUNCTION_INA_PING });
    }
}

void Power_Controller_PingMCP(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    _Error_Codes code = ConvHALError(HAL_I2C_IsDeviceReady(i2c_handle, MCP_I2C_ADDRESS, 3, 100));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_MCP,
                                    .code = code,
                                    .function = FUNCTION_MCP_PING });
    }
}

void Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    ctrl->i2c_handle = i2c_handle;
    ctrl->channels[POWER_CHANNEL_3V3] = channel_3v3_default;
    ctrl->channels[POWER_CHANNEL_5V] = channel_5v_default;
    ctrl->channels[POWER_CHANNEL_VARIABLE] = channel_var_default;

    _Error_Codes code = INA_Init(i2c_handle);
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_INA,
                                    .code = code,
                                    .function = FUNCTION_INA_INIT });
    }
}

void Power_Controller_SetVoltage(Power_Controller_t *ctrl, int32_t target_voltage_whole,
                                 uint32_t target_voltage_decimal)
{
    Power_Controller_Channel_t *chan = &ctrl->channels[POWER_CHANNEL_VARIABLE];

    if (target_voltage_whole < VOLTAGE_VARIABLE_MIN_WHOLE) {
        target_voltage_whole = VOLTAGE_VARIABLE_MIN_WHOLE;
        target_voltage_decimal = 0;
    }

    if (target_voltage_whole > VOLTAGE_VARIABLE_MAX_WHOLE) {
        target_voltage_whole = VOLTAGE_VARIABLE_MAX_WHOLE;
        target_voltage_decimal = 0;
    }

    MCP_Result_t result =
        MCP_SetSteps(ctrl->i2c_handle, target_voltage_whole, target_voltage_decimal);
    if (result.code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_INA,
                                    .code = result.code,
                                    .function = FUNCTION_MCP_SETSTEPS });
    }

    chan->target_voltage_whole = target_voltage_whole;
    chan->target_voltage_decimal = target_voltage_decimal;
    chan->variable.cur_dac_steps = result.steps;

    // this is to give enough time for the voltage to rise/fall
    // TODO: retest time it takes after redoing routing
    // TODO: retest when everything is working and load is connected
    // TODO: make software interrupt instead of blocking delay
    HAL_Delay(350);

    Power_Controller_UpdateCurValues(ctrl);
}

void Power_Controller_UpdateCurValues(Power_Controller_t *ctrl)
{
    Power_Controller_Channel_t *chan = &ctrl->channels[POWER_CHANNEL_VARIABLE];

    INA_Result_t result = INA_Read_Voltage(ctrl->i2c_handle);
    if (result.code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_INA,
                                    .code = result.code,
                                    .function = FUNCTION_INA_READ_V });
    }
    chan->variable.cur_voltage_whole = result.whole;
    chan->variable.cur_voltage_decimal = result.decimal;

    result = INA_Read_Current(ctrl->i2c_handle);
    if (result.code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_INA,
                                    .code = result.code,
                                    .function = FUNCTION_INA_READ_I });
    }
    chan->variable.cur_current_whole = result.whole;
    chan->variable.cur_current_decimal = result.decimal;
}

void Power_Controller_EnableOut(Power_Controller_t *ctrl, Power_Channels channel, bool enabled)
{
    Power_Controller_Channel_t *chan = &ctrl->channels[channel];

    HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, enabled ? 1 : 0);

    if (chan->channel_type == CHANNEL_TYPE_VARIABLE) {
        chan->variable.pid.p_gain = 50.0f;
        chan->variable.pid.i_gain = 5.0f;
        chan->variable.pid.acc_error = 0.0f;
        chan->variable.pid.prev_error = 0.0f;
        chan->variable.pid.last_time = HAL_GetTick();
        // TODO: start pid
        if (!enabled) {
            Power_Controller_SetVoltage(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE,
                                        VOLTAGE_VARIABLE_MIN_DECIMAL);
        } else {
            Power_Controller_SetVoltage(ctrl, chan->target_voltage_whole,
                                        chan->target_voltage_decimal);
        }
    }

    chan->output_enabled = enabled;
    Power_Controller_UpdateCurValues(ctrl);
}

// static uint8_t var_pi_start(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle);

/*
Your DAC is inverted (lower DAC = higher voltage). If error is positive (need more voltage), you
want to decrease DAC steps:

    int16_t adjustment = -(int16_t)step_adjustment;  // Negate for inverted

DAC Test this - if PI drives the wrong direction, add the negative sign.

Consider deadband to prevent oscillation:

    if (fabs(error) < 0.05f) {  // Within 50mV, don't adjust
        return ERROR_NONE;
    }
*/
// static uint8_t var_pi_start(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle)
// {
//     uint8_t err;
//     float error = chan->target_voltage - chan->cur_voltage;

//     uint32_t current_time = HAL_GetTick();
//     float delta_time = (current_time - chan->pid.last_time) / 1000.0f;

//     if (delta_time > 1.0f) {
//         chan->pid.last_time = current_time;
//         return ERROR_NONE;
//     }

//     chan->pid.last_time = current_time;

//     float proportion = chan->pid.p_gain * error;
//     chan->pid.acc_err += error * delta_time;

//     if (chan->pid.acc_err > 500.0f) {
//         chan->pid.acc_err = 500.0f;
//     }
//     if (chan->pid.acc_err < -500.0f) {
//         chan->pid.acc_err = -500.0f;
//     }

//     float integral = chan->pid.i_gain * chan->pid.acc_err;
//     float step_adjustment = proportion + integral;

//     int16_t adjustment = (int16_t)step_adjustment;
//     int16_t dac_steps = (int16_t)chan->cur_dac_steps + adjustment;

//     if (dac_steps < 0) {
//         dac_steps = 0;
//     }
//     if (dac_steps > 3250) {
//         dac_steps = 3250;
//     }

//     err = MCP_SetSteps(i2c_handle, (uint16_t)dac_steps);
//     if (err != ERROR_NONE) {
//         return err;
//     }

//     if (chan->cur_current > INA_CURRENT_TRESHOLD) {
//         Channel_VAR_EnableOutput(chan, false);
//         return 5;
//     }

//     return ERROR_NONE;
// }