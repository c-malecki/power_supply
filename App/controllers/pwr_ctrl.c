#include "pwr_ctrl.h"
#include "MCP4725.h"
#include "INA219.h"
#include "stm32f4xx_hal.h"
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

    MCP_Result_t mcp_res = MCP_WriteValue(ctrl->i2c_handle, MCP_VAL_12V);
    if (mcp_res.status != HAL_OK) {
        printf("MCP_WriteValue: Error: %d\r\n", err);
        return PWR_ERROR_I2C;
    }

    HAL_GPIO_WritePin(mosfet_port, mosfet_pin, GPIO_PIN_RESET);

    return PWR_OK;
}

Power_Status_t Power_SetVoltage(Power_Controller_t *ctrl, uint16_t value)
{
    MCP_Result_t mcp_result = MCP_WriteValue(ctrl->i2c_handle, value);
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

    if (current.value > 3.0) {
        Power_Enable(ctrl, false);
        return PWR_ERROR_OVERCURRENT;
    }

    return PWR_OK;
}

/**
 * @brief  Values were with ST-Link connected which reduces V slightly
 *
 *         ΔV = 1.38 - 1068 = -9.3V
 *         ΔDAC = 4095 - 511 = 3584 steps
 *
 *         Slope = -9.3 / 3584 = -0.002595 V/step (actual 0.0025948661)
 *
 *    *** 0 = ~12.00V
 *       511 = ~10.68
 *       1023 = ~9.29
 *   *** 1133 = ~9.05V
 *       1534 = ~7.91
 *       2047 = ~6.52
 *       2559 = ~5.14
 *       3071 = ~3.75
 *   *** 3236 = ~3.333
 *       3583 = ~2.36
 *       4095 = ~1.38
 *
 *        +/-100 to DAC = +/-0.2595V
 *
 * @retval HAL status
 */
void Power_SweepRange(Power_Controller_t *ctrl)
{
    uint16_t range[4] = { MCP_VAL_12V, MCP_VAL_9V, MCP_VAL_5V, MCP_VAL_3V3 };

    for (int i = 0; i < 4; i++) {
        MCP_Result_t mcp_result = MCP_WriteValue(ctrl->i2c_handle, range[i]);
        if (mcp_result.status != HAL_OK) {
            printf("Power_SweepRange: Error: %d\r\n", mcp_result.status);
            return;
        }

        INA_Result_t current = INA_ReadCurrent(ctrl->i2c_handle);
        INA_Result_t voltage = INA_ReadVoltage(ctrl->i2c_handle);
        float power = voltage.value * current.value;

        printf("V: %.4fV | I: %.4fmA | P: %.4fmW\n", voltage.value, current.value, power);

        HAL_Delay(3000);
    }
}