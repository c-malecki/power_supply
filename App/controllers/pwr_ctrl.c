#include "pwr_ctrl.h"
#include "MCP4725.h"
#include "INA219.h"
#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

Power_Status_t Power_Init(Power_Controller_t *ctrl, I2C_HandleTypeDef *handle,
                          GPIO_TypeDef *mosfet_port, uint16_t mosfet_pin)
{
    HAL_StatusTypeDef err;

    ctrl->i2c_handle = handle;
    ctrl->mosfet_port = mosfet_port;
    ctrl->mosfet_pin = mosfet_pin;
    ctrl->output_enabled = false;

    err = INA_Init(handle);
    if (err != HAL_OK) {
        printf("INA_Init: Error: %d\r\n", err);
        return PWR_ERROR_I2C;
    }

    MCP_Result_t mcp_res = MCP_WriteSteps(ctrl->i2c_handle, MCP_VAL_3V3);
    if (mcp_res.status != HAL_OK) {
        printf("MCP_WriteSteps: Error: %d\r\n", err);
        return PWR_ERROR_I2C;
    }

    HAL_GPIO_WritePin(mosfet_port, mosfet_pin, GPIO_PIN_RESET);

    return PWR_OK;
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
 * @param ctrl Pointer to the Power_Controller_t instance.
 * @param value 0 to 3232 : 12V tp 3.3V Value to change DAC's output voltage.
 * @retval Power_Status_t
 */
Power_Status_t Power_SetVoltage(Power_Controller_t *ctrl, uint16_t value)
{
    MCP_Result_t mcp_result = MCP_WriteSteps(ctrl->i2c_handle, value);
    if (mcp_result.status != HAL_OK) {
        return PWR_ERROR_I2C;
    }

    ctrl->mcp_target_voltage = mcp_result.value;

    return PWR_OK;
}

Power_Status_t Power_Enable(Power_Controller_t *ctrl, bool enable)
{
    HAL_GPIO_WritePin(ctrl->mosfet_port, ctrl->mosfet_pin, enable ? GPIO_PIN_SET : GPIO_PIN_RESET);

    ctrl->output_enabled = enable;

    return PWR_OK;
}

Power_Status_t Power_Update(Power_Controller_t *ctrl)
{
    INA_Result_t voltage = INA_ReadVoltage(ctrl->i2c_handle);
    INA_Result_t current = INA_ReadCurrent(ctrl->i2c_handle);

    if (voltage.status != HAL_OK || current.status != HAL_OK) {
        return PWR_ERROR_I2C;
    }

    ctrl->cur_voltage = voltage.value;
    ctrl->cur_current = current.value;

    if (current.value > 1.0) {
        Power_Enable(ctrl, false);
        return PWR_ERROR_OVERCURRENT;
    }

    return PWR_OK;
}

Power_Read_Result_t Power_Read(Power_Controller_t *ctrl)
{
    Power_Read_Result_t result;
    result.voltage = ctrl->cur_voltage;
    result.current = ctrl->cur_current;

    if (result.current > 1.0) {
        Power_Enable(ctrl, false);
    }

    return result;
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
void Power_SweepRange(Power_Controller_t *ctrl, uint8_t *count)
{
    uint16_t range[4] = { MCP_VAL_12V, MCP_VAL_9V, MCP_VAL_5V, MCP_VAL_3V3 };

    for (int i = 0; i < 4; i++) {

        MCP_Result_t mcp_result = MCP_WriteSteps(ctrl->i2c_handle, range[i]);
        if (mcp_result.status != HAL_OK) {
            printf("Power_SweepRange: Error: %d\r\n", mcp_result.status);
            return;
        }

        HAL_Delay(200);

        INA_Result_t current = INA_ReadCurrent(ctrl->i2c_handle);
        INA_Result_t voltage = INA_ReadVoltage(ctrl->i2c_handle);
        float power = voltage.value * current.value;

        printf("V: %.4fV | I: %.4fmA | P: %.4fmW\n", voltage.value, current.value, power);

        if (i == 3) {
            printf("\n");
        }

        HAL_Delay(1000);
    }

    (*count)++;
}