#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
//
#include "vdc.h"
#include "float.h"
#include "chan.h"
#include "MCP4725.h"
#include "INA219.h"

VDC_Rotary_t rot;

static uint8_t update_values(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle);
static uint8_t pi_start(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle);

void VDC_Channel_Init(VDC_Channel_t *chan)
{

    chan->target_voltage = 3.3f;
    chan->cur_dac_steps = MCP_VoltageToSteps(3.3f);
    chan->mosfet_port = MOSFET_VDC_GPIO_Port;
    chan->mosfet_pin = MOSFET_VDC_Pin;
    chan->output_enabled = false;

    rot.clk_port = ROT_CLK_GPIO_Port;
    rot.clk_pin = ROT_CLK_Pin;
    rot.dt_port = ROT_DT_GPIO_Port;
    rot.dt_pin = ROT_DT_Pin;
    rot.sw_port = ROT_SW_GPIO_Port;
    rot.sw_pin = ROT_SW_Pin;
    rot.last_clk = HAL_GPIO_ReadPin(ROT_CLK_GPIO_Port, ROT_CLK_Pin);
    rot.position = 0;
    rot.mode = VDC_ROTARY_MODE_OFF;

    chan->rot = &rot;
}

void VDC_Channel_Enable(VDC_Channel_t *chan, bool enabled)
{
    chan->pid->p_gain = 50.0f;
    chan->pid->i_gain = 5.0f;
    chan->pid->acc_err = 0.0f;
    chan->pid->prev_error = 0.0f;
    chan->pid->last_time = HAL_GetTick();
    chan->output_enabled = enabled;
    printf("VDC_Channel_Enable: %u\r\n\n", enabled);

    if (enabled) {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_SET);
    } else {
        HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, GPIO_PIN_RESET);
    }
}

uint8_t VDC_Channel_Set(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle, float target_voltage)
{
    uint8_t err;
    uint16_t steps = MCP_VoltageToSteps(target_voltage);

    err = MCP_SetSteps(i2c_handle, steps);
    if (err != HAL_OK) {
        return err;
    }

    chan->cur_dac_steps = steps;
    chan->target_voltage = target_voltage;

    printf("VDC_Channel_Set:\ncur_dac_steps %u\ntarget_voltage %.4f\r\n\n", chan->cur_dac_steps,
           chan->target_voltage);

    // TODO: How to do this non-blocking?
    HAL_Delay(200);

    return update_values(chan, i2c_handle);
}

static int8_t rotary_read(VDC_Rotary_t *rot)
{
    uint8_t clk = HAL_GPIO_ReadPin(rot->clk_port, rot->clk_pin);

    if (clk != rot->last_clk && clk == 0) {
        uint8_t dt = HAL_GPIO_ReadPin(rot->dt_port, rot->dt_pin);

        if (dt == 0) {
            rot->position++;
            rot->last_clk = clk;
            return 1;
        } else {
            rot->position--;
            rot->last_clk = clk;
            return -1;
        }
    }

    rot->last_clk = clk;

    return 0;
}

float pending_voltage = VDC_MIN_VOLTAGE;

uint8_t VDC_Rotary_Poll(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err = 0;
    int8_t change = rotary_read(chan->rot);

    if (rot.pressed) {
        rot.pressed = false;

        switch (rot.mode) {
        case VDC_ROTARY_MODE_OFF:
            rot.mode = VDC_ROTARY_MODE_ADJUST;
            pending_voltage = chan->cur_voltage;
            printf("off to adjust mode\r\n");
            break;

        case VDC_ROTARY_MODE_ADJUST:
            rot.mode = VDC_ROTARY_MODE_CONFIRM;
            printf("adjust to confirm mode\r\n");
            break;

        case VDC_ROTARY_MODE_CONFIRM:
            err = VDC_Channel_Set(chan, i2c_handle, pending_voltage);
            rot.mode = VDC_ROTARY_MODE_OFF;
            if (err != HAL_OK) {
                return err;
            }
            printf("confirm to off mode\r\n");
            break;
        }
    }

    printf("change: %d\r\n", change);

    if (change != 0 && rot.mode == VDC_ROTARY_MODE_ADJUST) {
        pending_voltage += change * 0.1f;

        if (pending_voltage < VDC_MIN_VOLTAGE) {
            pending_voltage = VDC_MIN_VOLTAGE;
        }
        if (pending_voltage > VDC_MAX_VOLTAGE) {
            pending_voltage = VDC_MAX_VOLTAGE;
        }
        printf("pending_voltage: %.4f\r\n", pending_voltage);
    }

    if (change != 0 && rot.mode == VDC_ROTARY_MODE_CONFIRM) {
        rot.mode = VDC_ROTARY_MODE_ADJUST;
        printf("confirm to adjust\r\n");
    }

    return err;
}

static uint8_t update_values(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle)
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
static uint8_t pi_start(VDC_Channel_t *chan, I2C_HandleTypeDef *i2c_handle)
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

    if (chan->cur_current > MAX_CURRENT) {
        VDC_Channel_Enable(chan, false);
        return 5;
    }

    return HAL_OK;
}