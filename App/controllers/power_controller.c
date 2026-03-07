#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
//
#include "power_controller.h"
#include "MCP4725.h"
#include "INA219.h"
#include "common.h"

static const Power_Controller_Channel_t channel_3v3_default = {
    .channel_type = CHANNEL_TYPE_FIXED,
    .mosfet_pin = MOSFET_CHAN_3V3_Pin,
    .mosfet_port = MOSFET_CHAN_3V3_GPIO_Port,
    .output_enabled = false,
    .target_voltage = VOLTAGE_3V3,
};

static const Power_Controller_Channel_t channel_5v_default = {
    .channel_type = CHANNEL_TYPE_FIXED,
    .mosfet_pin = MOSFET_CHAN_5V_Pin,
    .mosfet_port = MOSFET_CHAN_5V_GPIO_Port,
    .output_enabled = false,
    .target_voltage = VOLTAGE_5V,
};

static const Power_Controller_Channel_t channel_var_default = {
     .output_enabled = false,
    .channel_type = CHANNEL_TYPE_VARIABLE,
    .mosfet_pin = MOSFET_CHAN_VAR_Pin,
    .target_voltage = VOLTAGE_VARIABLE_MIN,
    .mosfet_port = MOSFET_CHAN_VAR_GPIO_Port,
    .variable = {
        .adjustment_state = {0},
        .cur_dac_steps = 0,
        .cur_voltage = 0.0f,
        .cur_current = 0.0f,
        .cur_power = 0.0f,
        .pid = {
            .p_gain = 0.0f,
            .i_gain = 0.0f,
            .acc_error = 0.0f,
            .prev_error = 0.0f,
            .last_time = 0,
        },
    }
};

Power_Controller_Ping_Result_t Power_Controller_PingPeripherals(I2C_HandleTypeDef *i2c_handle)
{
    Power_Controller_Ping_Result_t result = { .mcp = ERROR_NONE, .ina = ERROR_NONE };

    result.mcp = ConvHALError(HAL_I2C_IsDeviceReady(i2c_handle, MCP_I2C_ADDRESS, 3, 100));
    result.ina = ConvHALError(HAL_I2C_IsDeviceReady(i2c_handle, MCP_I2C_ADDRESS, 3, 100));

    return result;
}

_Error_Codes Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    ctrl->i2c_handle = i2c_handle;
    ctrl->channels[POWER_CHANNEL_3V3] = channel_3v3_default;
    ctrl->channels[POWER_CHANNEL_5V] = channel_5v_default;
    ctrl->channels[POWER_CHANNEL_VARIABLE] = channel_var_default;

    return INA_Init(i2c_handle);
}

Power_Controller_SetVariableVoltage_Result_t
Power_Controller_SetVariableVoltage(Power_Controller_t *ctrl, float target_voltage)
{
    Power_Controller_SetVariableVoltage_Result_t result = { .peripheral = PERIPHERAL_MCP,
                                                            .error = ERROR_NONE };

    Power_Controller_Channel_t chan = ctrl->channels[POWER_CHANNEL_VARIABLE];

    if (target_voltage < VOLTAGE_VARIABLE_MIN) {
        target_voltage = VOLTAGE_VARIABLE_MIN;
    }

    if (target_voltage > VOLTAGE_VARIABLE_MAX) {
        target_voltage = VOLTAGE_VARIABLE_MAX;
    }

    MCP_SetSteps_Result_t mcp_res = MCP_SetSteps(ctrl->i2c_handle, target_voltage);
    if (mcp_res.error != ERROR_NONE) {
        result.error = mcp_res.error;
        return result;
    }

    chan.target_voltage = target_voltage;
    chan.variable.cur_dac_steps = mcp_res.steps;

    // this is to give enough time for the voltage to rise/fall
    // TODO: retest time it takes after redoing routing
    // TODO: retest when everything is working and load is connected
    HAL_Delay(350);

    INA_Read_Result_t ina_res = INA_Read(ctrl->i2c_handle);
    if (ina_res.error != ERROR_NONE) {
        result.error = ina_res.error;
        result.peripheral = PERIPHERAL_INA;
        return result;
    }

    chan.variable.cur_voltage = ina_res.voltage;
    chan.variable.cur_current = ina_res.current;
    chan.variable.cur_power = ina_res.power;

    ctrl->channels[POWER_CHANNEL_VARIABLE] = chan;

    return result;
}

void Power_Controller_EnableChannel(Power_Controller_t *ctrl, Power_Channels channel, bool enabled)
{
    Power_Controller_Channel_t chan = ctrl->channels[channel];

    HAL_GPIO_WritePin(chan.mosfet_port, chan.mosfet_pin, enabled ? 1 : 0);

    if (chan.channel_type == CHANNEL_TYPE_VARIABLE) {
        chan.variable.pid.p_gain = 50.0f;
        chan.variable.pid.i_gain = 5.0f;
        chan.variable.pid.acc_error = 0.0f;
        chan.variable.pid.prev_error = 0.0f;
        chan.variable.pid.last_time = HAL_GetTick();
        // TODO: start pid
        if (!enabled) {
            Power_Controller_SetVariableVoltage(ctrl, VOLTAGE_VARIABLE_MIN);
        } else {
            Power_Controller_SetVariableVoltage(ctrl, chan.target_voltage);
        }
    }

    chan.output_enabled = enabled;

    ctrl->channels[channel] = chan;
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