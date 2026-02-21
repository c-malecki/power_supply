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

Channel_VDC_t chan_3v3;
Channel_VDC_t chan_5v;
Channel_VAR_t chan_var;
Channel_VAR_Rotary_t rotary;

static uint8_t var_update_values(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle);
static uint8_t var_pi_start(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle);

// Power Controller

uint8_t Power_Controller_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err;

    Channel_VDC_Init(&chan_3v3, 3.3f, MOSFET_3V3_GPIO_Port, MOSFET_3V3_Pin);
    Channel_VDC_Init(&chan_5v, 5.0f, MOSFET_5V_GPIO_Port, MOSFET_5V_Pin);
    Channel_VAR_Init(&chan_var);

    ctrl->chan_3v3 = &chan_3v3;
    ctrl->chan_5v = &chan_5v;
    ctrl->chan_var = &chan_var;

    err = INA_Init(i2c_handle);

    return err;
}

// Constant Voltage DC Channel

void Channel_VDC_Init(Channel_VDC_t *chan, float target_voltage, GPIO_TypeDef *mosfet_port,
                      uint16_t mosfet_pin)
{
    chan->target_voltage = target_voltage;
    chan->mosfet_port = mosfet_port;
    chan->mosfet_pin = mosfet_pin;
    chan->output_enabled = false;
}

void Channel_VDC_Enable_Output(Channel_VDC_t *chan, bool enabled)
{
    if (enabled) {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
    }
    chan->output_enabled = enabled;
}

// Variable Voltage DC Channel

void Channel_VAR_Init(Channel_VAR_t *chan)
{

    chan->target_voltage = 3.3f;
    chan->cur_dac_steps = MCP_VoltageToSteps(3.3f);
    chan->mosfet_port = MOSFET_VDC_GPIO_Port;
    chan->mosfet_pin = MOSFET_VDC_Pin;
    chan->output_enabled = false;

    rotary.clk_port = ROT_CLK_GPIO_Port;
    rotary.clk_pin = ROT_CLK_Pin;
    rotary.dt_port = ROT_DT_GPIO_Port;
    rotary.dt_pin = ROT_DT_Pin;
    rotary.sw_port = ROT_SW_GPIO_Port;
    rotary.sw_pin = ROT_SW_Pin;
    rotary.last_clk = HAL_GPIO_ReadPin(ROT_CLK_GPIO_Port, ROT_CLK_Pin);
    rotary.position = 0;
    rotary.mode = ROTARY_MODE_OFF;

    chan->rotary = &rotary;
}

void Channel_VAR_Enable_Output(Channel_VAR_t *chan, bool enabled)
{
    chan->pid->p_gain = 50.0f;
    chan->pid->i_gain = 5.0f;
    chan->pid->acc_err = 0.0f;
    chan->pid->prev_error = 0.0f;
    chan->pid->last_time = HAL_GetTick();
    chan->output_enabled = enabled;
    printf("Channel_VAR_Enable: %u\r\n\n", enabled);

    if (enabled) {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
    }
}

uint8_t Channel_VAR_Set_Voltage(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle,
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

    printf("Channel_VAR_Set:\ncur_dac_steps %u\ntarget_voltage %.4f\r\n\n", chan->cur_dac_steps,
           chan->target_voltage);

    // TODO: How to do this non-blocking?
    HAL_Delay(200);

    return var_update_values(chan, i2c_handle);
}

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

float pending_voltage = CHANNEL_VAR_MIN_VOLTAGE;

uint8_t Channel_VAR_Poll_Rotary(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err = 0;
    int8_t change = rotary_read(chan->rotary);

    if (rotary.pressed) {
        rotary.pressed = false;

        switch (rotary.mode) {
        case ROTARY_MODE_OFF:
            rotary.mode = ROTARY_MODE_ADJUST;
            pending_voltage = chan->cur_voltage;
            printf("off to adjust mode\r\n");
            break;

        case ROTARY_MODE_ADJUST:
            rotary.mode = ROTARY_MODE_CONFIRM;
            printf("adjust to confirm mode\r\n");
            break;

        case ROTARY_MODE_CONFIRM:
            err = Channel_VAR_Set_Voltage(chan, i2c_handle, pending_voltage);
            rotary.mode = ROTARY_MODE_OFF;
            if (err != HAL_OK) {
                return err;
            }
            printf("confirm to off mode\r\n");
            break;
        }
    }

    printf("change: %d\r\n", change);

    if (change != 0 && rotary.mode == ROTARY_MODE_ADJUST) {
        pending_voltage += change * 0.1f;

        if (pending_voltage < CHANNEL_VAR_MIN_VOLTAGE) {
            pending_voltage = CHANNEL_VAR_MIN_VOLTAGE;
        }
        if (pending_voltage > CHANNEL_VAR_MAX_VOLTAGE) {
            pending_voltage = CHANNEL_VAR_MAX_VOLTAGE;
        }
        printf("pending_voltage: %.4f\r\n", pending_voltage);
    }

    if (change != 0 && rotary.mode == ROTARY_MODE_CONFIRM) {
        rotary.mode = ROTARY_MODE_ADJUST;
        printf("confirm to adjust\r\n");
    }

    return err;
}

static uint8_t var_update_values(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle)
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
static uint8_t var_pi_start(Channel_VAR_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err;
    float error = chan->target_voltage - chan->cur_voltage;

    uint32_t current_time = HAL_GetTick();
    float delta_time = (current_time - chan->pid->last_time) / 1000.0f;

    if (delta_time > 1.0f) {
        chan->pid->last_time = current_time;
        return HAL_OK;
    }

    chan->pid->last_time = current_time;

    float proportion = chan->pid->p_gain * error;
    chan->pid->acc_err += error * delta_time;

    if (chan->pid->acc_err > 500.0f) {
        chan->pid->acc_err = 500.0f;
    }
    if (chan->pid->acc_err < -500.0f) {
        chan->pid->acc_err = -500.0f;
    }

    float integral = chan->pid->i_gain * chan->pid->acc_err;
    float step_adjustment = proportion + integral;

    int16_t adjustment = (int16_t)step_adjustment;
    int16_t dac_steps = (int16_t)chan->cur_dac_steps + adjustment;

    if (dac_steps < 0) {
        dac_steps = 0;
    }
    if (dac_steps > 3250) {
        dac_steps = 3250;
    }

    err = MCP_SetSteps(i2c_handle, (uint16_t)dac_steps);
    if (err != HAL_OK) {
        return err;
    }

    if (chan->cur_current > CHANNEL_VAR_MAX_CURRENT) {
        Channel_VAR_Enable_Output(chan, false);
        return 5;
    }

    return HAL_OK;
}