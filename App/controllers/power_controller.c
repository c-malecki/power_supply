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

static Channel_VDC_t chan_3v3;
static Channel_VDC_t chan_5v;
static Channel_VAR_t chan_var;

static uint8_t var_pi_start(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle);
void init_vdc_chan(Channel_VDC_t *chan, const Channel_InitStruct *cfg);
void init_var_chan(Channel_VAR_t *chan, const Channel_InitStruct *cfg);

const Channel_InitStruct cfg_3v3 = { 3.3f, MOSFET_3V3_GPIO_Port, MOSFET_3V3_Pin, LED_3V3_GPIO_Port,
                                     LED_3V3_Pin };
const Channel_InitStruct cfg_5v = { 5.0f, MOSFET_5V_GPIO_Port, MOSFET_5V_Pin, LED_5V_GPIO_Port,
                                    LED_5V_Pin };
const Channel_InitStruct cfg_var = { 3.3f, MOSFET_VAR_GPIO_Port, MOSFET_VAR_Pin, LED_VAR_GPIO_Port,
                                     LED_VAR_Pin };

// Power Controller

uint8_t Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    init_vdc_chan(&chan_3v3, &cfg_3v3);
    init_vdc_chan(&chan_5v, &cfg_5v);
    init_var_chan(&chan_var, &cfg_var);

    ctrl->chan_3v3 = &chan_3v3;
    ctrl->chan_5v = &chan_5v;
    ctrl->chan_var = &chan_var;

    return INA_Init(i2c_handle);
}

void Channel_VDC_EnableOutput(Channel_VDC_t *chan, bool enabled)
{
    if (enabled) {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(chan->led_port, chan->led_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(chan->led_port, chan->led_pin, GPIO_PIN_RESET);
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
        HAL_GPIO_WritePin(chan->led_port, chan->led_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(chan->led_port, chan->led_pin, GPIO_PIN_RESET);
    }

    chan->output_enabled = enabled;
}

uint8_t Channel_VAR_UpdateValues(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err;
    float voltage, current, power;

    err = INA_Read(i2c_handle, &voltage, &current);
    if (err != HAL_OK) {
        return err;
    }

    power = voltage * current;

    chan->cur_voltage = voltage;
    chan->cur_current = current;
    chan->cur_power = power;

    return err;
}

uint8_t Channel_VAR_SetVoltage(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle,
                               float target_voltage)
{
    uint8_t err;
    uint16_t steps = MCP_VoltageToSteps(target_voltage);

    err = MCP_SetSteps(i2c_handle, steps);
    if (err != HAL_OK) {
        return err;
    }

    chan->cur_dac_steps = steps;
    chan->target_voltage = target_voltage;

    HAL_Delay(320);

    return Channel_VAR_UpdateValues(chan, i2c_handle);
}

// Channel VAR Rotary

static int8_t rotary_read(Channel_VAR_Rotary_t *rotary)
{
    uint8_t clk = HAL_GPIO_ReadPin(rotary->clk_port, rotary->clk_pin);

    if (clk != rotary->last_clk && clk == 0) {
        uint8_t dt = HAL_GPIO_ReadPin(rotary->dt_port, rotary->dt_pin);

        if (dt == 0) {
            rotary->position++;
            rotary->last_clk = clk;
            return 1;
        } else {
            rotary->position--;
            rotary->last_clk = clk;
            return -1;
        }
    }

    rotary->last_clk = clk;

    return 0;
}

float pending_voltage = 0.0f;

uint8_t Channel_VAR_PollRotary(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err = 0;
    int8_t change = rotary_read(&chan->rotary);

    if (chan->rotary.pressed) {
        chan->rotary.pressed = false;

        switch (chan->rotary.mode) {
        case ROTARY_MODE_OFF:
            chan->rotary.mode = ROTARY_MODE_ADJUST;
            pending_voltage = chan->cur_voltage;
            printf("off to adjust mode\r\n");
            break;

        case ROTARY_MODE_ADJUST:
            chan->rotary.mode = ROTARY_MODE_CONFIRM;
            printf("adjust to confirm mode\r\n");
            break;

        case ROTARY_MODE_CONFIRM:
            err = Channel_VAR_SetVoltage(chan, i2c_handle, pending_voltage);
            chan->rotary.mode = ROTARY_MODE_OFF;
            if (err != HAL_OK) {
                return err;
            }
            printf("confirm to off mode\r\n");
            break;
        }
    }

    printf("change: %d\r\n", change);

    if (change != 0 && chan->rotary.mode == ROTARY_MODE_ADJUST) {
        pending_voltage += change * 0.1f;

        if (pending_voltage < MCP_MIN_VOLTAGE) {
            pending_voltage = MCP_MIN_VOLTAGE;
        }
        if (pending_voltage > MCP_MAX_VOLTAGE) {
            pending_voltage = MCP_MAX_VOLTAGE;
        }
        printf("pending_voltage: %.4f\r\n", pending_voltage);
    }

    if (change != 0 && chan->rotary.mode == ROTARY_MODE_CONFIRM) {
        chan->rotary.mode = ROTARY_MODE_ADJUST;
        printf("confirm to adjust\r\n");
    }

    return err;
}

void init_vdc_chan(Channel_VDC_t *chan, const Channel_InitStruct *cfg)
{
    chan->target_voltage = cfg->target_voltage;
    chan->mosfet_port = cfg->mosfet_port;
    chan->mosfet_pin = cfg->mosfet_pin;
    chan->led_port = cfg->led_port;
    chan->led_pin = cfg->led_pin;
    chan->output_enabled = false;
}

void init_var_chan(Channel_VAR_t *chan, const Channel_InitStruct *cfg)
{
    chan->target_voltage = cfg->target_voltage;
    chan->mosfet_port = cfg->mosfet_port;
    chan->mosfet_pin = cfg->mosfet_pin;
    chan->led_port = cfg->led_port;
    chan->led_pin = cfg->led_pin;

    chan->cur_dac_steps = MCP_VoltageToSteps(cfg->target_voltage);
    chan->output_enabled = false;

    chan->rotary.clk_port = ROTARY_CLK_GPIO_Port;
    chan->rotary.clk_pin = ROTARY_CLK_Pin;
    chan->rotary.dt_port = ROTARY_DT_GPIO_Port;
    chan->rotary.dt_pin = ROTARY_DT_Pin;
    chan->rotary.sw_port = ROTARY_SW_GPIO_Port;
    chan->rotary.sw_pin = ROTARY_SW_Pin;
    chan->rotary.last_clk = HAL_GPIO_ReadPin(ROTARY_CLK_GPIO_Port, ROTARY_CLK_Pin);
    chan->rotary.position = 0;
    chan->rotary.mode = ROTARY_MODE_OFF;

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