#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "gpio.h"
#include "main.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
//
#include "ina219.h"
#include "mcp4725.h"
#include "common.h"
#include "power_controller.h"

void Pwr_Chan_SetVarV(Pwr_Ctrl_t *ctrl, int32_t target_voltage_w, uint32_t target_voltage_d);
void Power_Controller_UpdateVarValues(Pwr_Ctrl_t *ctrl);

static const Pwr_Chan_t channel_3v3_default = {
    .toggle_pending = false,
    .output_on = false,
    .mosfet_pin = GPIO_MOSFET_3V3_Pin,
    .mosfet_port = GPIO_MOSFET_3V3_GPIO_Port,
};

static const Pwr_Chan_t channel_5v_default = {
    .toggle_pending = false,
    .output_on = false,
    .mosfet_pin = GPIO_MOSFET_5V_Pin,
    .mosfet_port = GPIO_MOSFET_5V_GPIO_Port,
};

static const Pwr_Chan_t channel_var_default = {
    .toggle_pending = false,
    .output_on = false,
    .mosfet_pin = GPIO_MOSFET_VAR_Pin,
    .mosfet_port = GPIO_MOSFET_VAR_GPIO_Port,
};

static const Pwr_Var_Control_t var_ctrls_default = {
    .pid_p_gain = 0.0f,
    .pid_i_gain = 0.0f,
    .pid_acc_error = 0.0f,
    .pid_prev_error = 0.0f,
    .target_voltage_w = VOLTAGE_VARIABLE_MIN_WHOLE,
    .target_voltage_d = VOLTAGE_VARIABLE_MIN_DECIMAL,
    .pid_last_time = 0,
    .mcp = { .i2c_addr = MCP_I2C_ADDR, .steps = 0 }
};

static const Pwr_Buck_t buck_main_default = { .on = true,
                                              .ina = {
                                                  .i2c_addr = INA_I2C_ADDR_MAIN,
                                                  .last_read = 0,
                                                  .voltage_w = 0,
                                                  .voltage_d = 0,
                                                  .current_w = 0,
                                                  .current_d = 0,
                                              } };

static const Pwr_Buck_t buck_6v5_default = { .on = false,
                                             .gpio_pin = GPIO_BUCK_6V5_Pin,
                                             .gpio_port = GPIO_BUCK_6V5_GPIO_Port,
                                             .ina = {
                                                 .i2c_addr = INA_I2C_ADDR_6V5,
                                                 .last_read = 0,
                                                 .voltage_w = 0,
                                                 .voltage_d = 0,
                                                 .current_w = 0,
                                                 .current_d = 0,
                                             } };

static const Pwr_Buck_t buck_12v_default = { .on = false,
                                             .gpio_pin = GPIO_BUCK_12V_Pin,
                                             .gpio_port = GPIO_BUCK_12V_GPIO_Port,
                                             .ina = {
                                                 .i2c_addr = INA_I2C_ADDR_12V,
                                                 .last_read = 0,
                                                 .voltage_w = 0,
                                                 .voltage_d = 0,
                                                 .current_w = 0,
                                                 .current_d = 0,
                                             } };

static const Pwr_Buck_t buck_var_default = { .on = false,
                                             .gpio_pin = GPIO_BUCK_VAR_Pin,
                                             .gpio_port = GPIO_BUCK_VAR_GPIO_Port,
                                             .ina = {
                                                 .i2c_addr = INA_I2C_ADDR_VAR,
                                                 .last_read = 0,
                                                 .voltage_w = 0,
                                                 .voltage_d = 0,
                                                 .current_w = 0,
                                                 .current_d = 0,
                                             } };

void Pwr_Ctrl_Init(Pwr_Ctrl_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    ctrl->i2c_handle = i2c_handle;
    ctrl->channels[PWR_CHAN_3V3] = channel_3v3_default;
    ctrl->channels[PWR_CHAN_5V] = channel_5v_default;
    ctrl->channels[PWR_CHAN_VARV] = channel_var_default;
    ctrl->bucks[PWR_BUCK_MAIN] = buck_main_default;
    ctrl->bucks[PWR_BUCK_6V5] = buck_6v5_default;
    ctrl->bucks[PWR_BUCK_12V] = buck_12v_default;
    ctrl->bucks[PWR_BUCK_VARV] = buck_var_default;
    ctrl->var_ctrls = var_ctrls_default;
}

void Pwr_Ctrl_Ping(Pwr_Ctrl_t *ctrl)
{
    _Error_Codes code;
    // TODO: update functions enum
    for (uint8_t i = 0; i < PWR_BUCK_COUNT; i++) {
        Pwr_Buck_t buck = ctrl->bucks[i];

        code = ConvHALError(HAL_I2C_IsDeviceReady(ctrl->i2c_handle, buck.ina.i2c_addr, 3, 100));
        if (code != ERROR_NONE) {
            ctrl->error_cb(ctrl->error_ctx,
                           (_Error_t) { .controller = CONTROLLER_POWER,
                                        .peripheral = PERIPHERAL_INA_MAIN,
                                        .code = code,
                                        .function = FUNCTION_INA_PING });
        }
    }

    code = ConvHALError(HAL_I2C_IsDeviceReady(ctrl->i2c_handle, MCP_I2C_ADDR, 3, 100));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_MCP,
                                    .code = code,
                                    .function = FUNCTION_MCP_PING });
    }
}

void Pwr_Ctrl_Run(Pwr_Ctrl_t *ctrl)
{
    _Error_Codes code;

    for (uint8_t i = 0; i < PWR_BUCK_COUNT; i++) {
        Pwr_Buck_t buck = ctrl->bucks[i];

        code = INA_Init(&buck.ina, ctrl->i2c_handle);
        if (code != ERROR_NONE) {
            ctrl->error_cb(ctrl->error_ctx,
                           (_Error_t) { .controller = CONTROLLER_POWER,
                                        .peripheral = PERIPHERAL_INA_MAIN,
                                        .code = code,
                                        .function = FUNCTION_INA_INIT });
        }
    }
}

void Pwr_Buck_Toggle(Pwr_Ctrl_t *ctrl, Pwr_Buck buck)
{
    Pwr_Buck_t *b = &ctrl->bucks[buck];
    bool new_state = !b->on;
    HAL_GPIO_WritePin(b->gpio_port, b->gpio_pin, new_state);
    b->on = new_state;
}

void Pwr_Chan_Toggle(Pwr_Ctrl_t *ctrl, Pwr_Chan channel)
{
    Pwr_Chan_t *chan = &ctrl->channels[channel];
    uint8_t new_state = !chan->output_on;

    HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, new_state);

    if (channel == PWR_CHAN_VARV) {
        ctrl->var_ctrls.pid_p_gain = 50.0f;
        ctrl->var_ctrls.pid_i_gain = 5.0f;
        ctrl->var_ctrls.pid_acc_error = 0.0f;
        ctrl->var_ctrls.pid_prev_error = 0.0f;
        ctrl->var_ctrls.pid_last_time = HAL_GetTick();
        // TODO: start pid
        if (!chan->output_on) {
            Pwr_Chan_SetVarV(ctrl, VOLTAGE_VARIABLE_MIN_WHOLE, VOLTAGE_VARIABLE_MIN_DECIMAL);
        } else {
            Pwr_Chan_SetVarV(ctrl, ctrl->var_ctrls.target_voltage_w,
                             ctrl->var_ctrls.target_voltage_d);
        }
    }

    chan->toggle_pending = false;
    chan->output_on = new_state;
}

void Pwr_Chan_SetVarV(Pwr_Ctrl_t *ctrl, int32_t target_voltage_w, uint32_t target_voltage_d)
{
    if (target_voltage_w < VOLTAGE_VARIABLE_MIN_WHOLE) {
        target_voltage_w = VOLTAGE_VARIABLE_MIN_WHOLE;
        target_voltage_d = 0;
    }

    if (target_voltage_w > VOLTAGE_VARIABLE_MAX_WHOLE) {
        target_voltage_w = VOLTAGE_VARIABLE_MAX_WHOLE;
        target_voltage_d = 0;
    }

    _Error_Codes code = MCP_VoltageToSteps(&ctrl->var_ctrls.mcp, ctrl->i2c_handle, target_voltage_w,
                                           target_voltage_d);
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_POWER,
                                    .peripheral = PERIPHERAL_MCP,
                                    .code = code,
                                    .function = FUNCTION_MCP_V_TO_S });
    }

    ctrl->var_ctrls.target_voltage_w = target_voltage_w;
    ctrl->var_ctrls.target_voltage_d = target_voltage_d;
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
//     float delta_time = (current_time - chan->pid.pid_last_time) / 1000.0f;

//     if (delta_time > 1.0f) {
//         chan->pid.pid_last_time = current_time;
//         return ERROR_NONE;
//     }

//     chan->pid.pid_last_time = current_time;

//     float proportion = chan->pid.pid_p_gain * error;
//     chan->pid.acc_err += error * delta_time;

//     if (chan->pid.acc_err > 500.0f) {
//         chan->pid.acc_err = 500.0f;
//     }
//     if (chan->pid.acc_err < -500.0f) {
//         chan->pid.acc_err = -500.0f;
//     }

//     float integral = chan->pid.pid_i_gain * chan->pid.acc_err;
//     float step_adjustment = proportion + integral;

//     int16_t adjustment = (int16_t)step_adjustment;
//     int16_t dac_steps = (int16_t)chan->cur_dac_steps + adjustment;

//     if (dac_steps < 0) {
//         dac_steps = 0;
//     }
//     if (dac_steps > 3250) {
//         dac_steps = 3250;
//     }

//     err = MCP_VoltageToSteps(i2c_handle, (uint16_t)dac_steps);
//     if (err != ERROR_NONE) {
//         return err;
//     }

//     if (chan->cur_current > INA_CURRENT_TRESHOLD) {
//         Channel_VAR_EnableOutput(chan, false);
//         return 5;
//     }

//     return ERROR_NONE;
// }