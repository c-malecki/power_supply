#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
//
#include "power_controller.h"
#include "float.h"
#include "MCP4725.h"
#include "INA219.h"

static uint8_t var_pi_start(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle);
void init_vdc_chan(Channel_VDC_t *chan, const Channel_InitStruct *cfg,
                   I2C_HandleTypeDef *i2c_handle);
void init_var_chan(Channel_VAR_t *chan, const Channel_InitStruct *cfg,
                   I2C_HandleTypeDef *i2c_handle);

const Channel_InitStruct cfg_3v3 = { 3.3f, MOSFET_CHAN_3V3_GPIO_Port, MOSFET_CHAN_3V3_Pin };
const Channel_InitStruct cfg_5v = { 5.0f, MOSFET_CHAN_5V_GPIO_Port, MOSFET_CHAN_5V_Pin };
const Channel_InitStruct cfg_var = { 3.3f, MOSFET_CHAN_VAR_GPIO_Port, MOSFET_CHAN_VAR_Pin };

uint8_t Power_Controller_MCP_Ping(I2C_HandleTypeDef *i2c_handle)
{
    return (uint8_t)HAL_I2C_IsDeviceReady(i2c_handle, MCP_I2C_ADDR, 3, 100);
}

uint8_t Power_Controller_INA_Ping(I2C_HandleTypeDef *i2c_handle)
{
    return (uint8_t)HAL_I2C_IsDeviceReady(i2c_handle, MCP_I2C_ADDR, 3, 100);
}

uint8_t Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    ctrl->i2c_handle = i2c_handle;

    init_vdc_chan(&ctrl->chan_3v3, &cfg_3v3, i2c_handle);
    init_vdc_chan(&ctrl->chan_5v, &cfg_5v, i2c_handle);
    init_var_chan(&ctrl->chan_var, &cfg_var, i2c_handle);

    return INA_Init(i2c_handle);
}

void Power_Controller_State_Print(Power_Controller_t *ctrl)
{
    printf("POWER:\n\n3V3:\n");
}

void Channel_VDC_EnableOutput(Channel_VDC_t *chan, bool enabled)
{
    if (enabled) {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
    }
    chan->output_enabled = enabled;
}

void Channel_VAR_EnableOutput(Channel_VAR_t *chan, bool enabled)
{
    chan->pid.p_gain = 50.0f;
    chan->pid.i_gain = 5.0f;
    chan->pid.acc_err = 0.0f;
    chan->pid.prev_error = 0.0f;
    chan->pid.last_time = HAL_GetTick();

    if (enabled) {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
    }

    chan->output_enabled = enabled;
}

uint8_t Channel_VAR_UpdateValues(Power_Controller_t *ctrl, Channel_VAR_t *chan)
{
    uint8_t err;
    float voltage, current, power;

    err = INA_Read(ctrl->i2c_handle, &voltage, &current);
    if (err != HAL_OK) {
        return err;
    }

    power = voltage * current;

    chan->cur_voltage = voltage;
    chan->cur_current = current;
    chan->cur_power = power;

    return err;
}

uint8_t Channel_VAR_SetVoltage(Power_Controller_t *ctrl, Channel_VAR_t *chan, float target_voltage)
{
    uint8_t err;
    uint16_t steps = MCP_VoltageToSteps(target_voltage);

    err = MCP_SetSteps(ctrl->i2c_handle, steps);
    if (err != HAL_OK) {
        return err;
    }

    chan->cur_dac_steps = steps;
    chan->target_voltage = target_voltage;

    HAL_Delay(320);

    return Channel_VAR_UpdateValues(ctrl, chan);
}

void init_vdc_chan(Channel_VDC_t *chan, const Channel_InitStruct *cfg,
                   I2C_HandleTypeDef *i2c_handle)
{
    chan->target_voltage = cfg->target_voltage;
    chan->mosfet_port = cfg->mosfet_port;
    chan->mosfet_pin = cfg->mosfet_pin;
    chan->output_enabled = false;
}

void init_var_chan(Channel_VAR_t *chan, const Channel_InitStruct *cfg,
                   I2C_HandleTypeDef *i2c_handle)
{
    chan->target_voltage = cfg->target_voltage;
    chan->mosfet_port = cfg->mosfet_port;
    chan->mosfet_pin = cfg->mosfet_pin;

    chan->cur_dac_steps = MCP_VoltageToSteps(cfg->target_voltage);
    chan->output_enabled = false;

    chan->pid.p_gain = 50.0f;
    chan->pid.i_gain = 5.0f;
    chan->pid.acc_err = 0.0f;
    chan->pid.prev_error = 0.0f;
    chan->pid.last_time = HAL_GetTick();
}

/*
Your DAC is inverted (lower DAC = higher voltage). If error is positive (need more voltage), you
want to decrease DAC steps:

    int16_t adjustment = -(int16_t)step_adjustment;  // Negate for inverted

DAC Test this - if PI drives the wrong direction, add the negative sign.

Consider deadband to prevent oscillation:

    if (fabs(error) < 0.05f) {  // Within 50mV, don't adjust
        return HAL_OK;
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
//         return HAL_OK;
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
//     if (err != HAL_OK) {
//         return err;
//     }

//     if (chan->cur_current > INA_MAX_CURRENT) {
//         Channel_VAR_EnableOutput(chan, false);
//         return 5;
//     }

//     return HAL_OK;
// }