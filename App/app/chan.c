#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "float.h"
#include "chan.h"
#include "MCP4725.h"
#include "INA219.h"
#include "stm32f4xx_hal_def.h"

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

    err = MCP_SetSteps(i2c_handle, target_voltage);
    if (err != HAL_OK) {
        return err;
    }

    chan->target_voltage = target_voltage;

    return err;
}

uint8_t PWR_Chan_Update(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err;
    float voltage, current;

    err = INA_ReadVoltage(i2c_handle, &voltage);
    if (err != HAL_OK) {
        return err;
    }

    err = INA_ReadCurrent(i2c_handle, &current);
    if (err != HAL_OK) {
        return err;
    }

    chan->cur_voltage = voltage;
    chan->cur_current = current;

    return err;
}

uint8_t PWR_Chan_PI_Start(PWR_Chan_t *chan, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err = PWR_Chan_Update(chan, i2c_handle);
    if (err != HAL_OK) {
        return err;
    }

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

/**
* Test values were with ST-Link connected and 1kR as load
*
* V: 12.0280V | I: 11.9000mA | P: 143.1332mW
* V: 9.0240V | I: 8.9000mA | P: 80.3136mW
* V: 5.0640V | I: 5.0000mA | P: 25.3200mW
* V: 3.3080V | I: 3.1000mA | P: 10.2548mW

* V: 12.0280V | I: 11.9000mA | P: 143.1332mW
* V: 9.0200V | I: 8.9000mA | P: 80.2780mW
* V: 5.0560V | I: 4.6000mA | P: 23.2576mW
* V: 3.3040V | I: 3.2000mA | P: 10.5728mW

* V: 12.0280V | I: 11.9000mA | P: 143.1332mW
* V: 9.0160V | I: 9.2000mA | P: 82.9472mW
* V: 5.0520V | I: 4.7000mA | P: 23.7444mW
* V: 3.3080V | I: 3.4000mA | P: 11.2472mW
*
*/
// void PWR_SweepRange(PWR_Ctrl_t *ctrl, uint8_t *count)
// {
//     uint16_t range[4] = { MCP_VAL_12V, MCP_VAL_9V, MCP_VAL_5V, MCP_VAL_3V3 };

//     for (int i = 0; i < 4; i++) {

//         MCP_Result_t mcp_result = MCP_SetSteps(ctrl->i2c_handle, range[i]);
//         if (mcp_result.status != HAL_OK) {
//             printf("Power_SweepRange: Error: %d\r\n", mcp_result.status);
//             return;
//         }

//         HAL_Delay(200);

//         INA_Result_t current = INA_ReadCurrent(ctrl->i2c_handle);
//         INA_Result_t voltage = INA_ReadVoltage(ctrl->i2c_handle);
//         float power = voltage.value * current.value;

//         printf("V: %.4fV | I: %.4fmA | P: %.4fmW\n", voltage.value, current.value, power);

//         if (i == 3) {
//             printf("\n");
//         }

//         HAL_Delay(1000);
//     }

//     (*count)++;
// }