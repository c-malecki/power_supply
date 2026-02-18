#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include "float.h"
#include "chan.h"
#include "MCP4725.h"
#include "INA219.h"
#include "stm32f4xx_hal_def.h"

static uint8_t update_values(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle);
static uint8_t pi_start(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle);

// TODO: set pins for mosfets
void PWR_Chan_Init(PWR_Chan_t *chan, float target_voltage, GPIO_TypeDef *mosfet_port,
                   uint16_t mosfet_pin)
{
    chan->target_voltage = target_voltage;
    chan->dac_steps = MCP_VoltageToSteps(target_voltage);
    chan->mosfet_port = mosfet_port;
    chan->mosfet_pin = mosfet_pin;
    chan->output_enabled = false;
}

void PWR_Chan_Enable(PWR_Chan_t *chan, bool enabled)
{
    HAL_GPIO_WritePin(chan->mosfet_port, chan->mosfet_pin, enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
    chan->pid->p_gain = 300.0f;
    chan->pid->i_gain = 50.0f;
    chan->pid->acc_err = 0.0f;
    chan->pid->prev_error = 0.0f;
    chan->pid->last_time = HAL_GetTick();
    chan->output_enabled = enabled;
}

/**
 *  @brief Changes the DAC's output voltage to the feedback pin via voltage injection
 *  ΔV = 3.3V - 12V = -8.7V
 *  ΔStep = -8.7 / 3232 = 0.00269183
 *                        ^ ^^^^^
 *                        using float so limited to 7 digits
 *
 *  0    = ~12.00V
 *  1130 = ~9.00V
 *  2590 = ~5.00
 *  3232 = ~3.30
 *
 * @param ctrl Pointer to the PWR_Ctrl_t instance.
 * @param value 0 to 3232 : 12V tp 3.3V Value to change DAC's output voltage.
 * @retval PWR_Status_t
 */
uint8_t PWR_Chan_Set(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle, float target_voltage)
{
    uint8_t err;
    uint16_t steps = MCP_VoltageToSteps(target_voltage);

    err = MCP_SetSteps(i2c_handle, steps);
    if (err != HAL_OK) {
        return err;
    }

    chan->dac_steps = steps;
    chan->target_voltage = target_voltage;

    // TODO: How to do this non-blocking?
    HAL_Delay(200);

    return update_values(chan, i2c_handle);
}

static uint8_t update_values(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle)
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

static uint8_t pi_start(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err;
    float error = chan->target_voltage - chan->cur_voltage;

    uint32_t current_time = HAL_GetTick();

    float delta_time = (current_time - chan->pid->last_time) / 1000.0f;
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
    int16_t dac_steps = (int16_t)chan->dac_steps + adjustment;

    if (dac_steps < 0) {
        dac_steps = 0;
    }
    if (dac_steps > 3232) {
        dac_steps = 3232;
    }

    err = MCP_SetSteps(i2c_handle, (uint16_t)dac_steps);
    if (err != HAL_OK) {
        return err;
    }

    if (chan->cur_current > MAX_CURRENT) {
        PWR_Chan_Enable(chan, false);
        return 5;
    }

    return HAL_OK;
}