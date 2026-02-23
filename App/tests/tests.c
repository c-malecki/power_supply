#include "main.h"
#include "power_controller.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "tests.h"
#include "MCP4725.h"
#include "INA219.h"

void print_readings(Channel_VAR_t *chan);

App_Status_t Test_ChannelVar(App_t *app)
{
    App_Status_t status;
    status.ctrl = APP_CTRL;
    status.err = 0;

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MAX_VOLTAGE);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    Channel_VAR_EnableOutput(app->pwr_ctrl->chan_var, true);

    HAL_Delay(200);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MIN_VOLTAGE);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    HAL_Delay(200);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MAX_VOLTAGE);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    HAL_Delay(200);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MIN_VOLTAGE);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    HAL_Delay(200);

    Channel_VAR_EnableOutput(app->pwr_ctrl->chan_var, true);

    print_readings(app->pwr_ctrl->chan_var);

    return status;
}

void print_readings(Channel_VAR_t *chan)
{
    HAL_Delay(200);
    printf("Channel_VAR:\nenabled %u\ntarget_v %.4fV\ncur_v %.4fV\ncur_dac_steps %u\ncur_i "
           "%.4fmA\r\n\n",
           chan->output_enabled, chan->target_voltage, chan->cur_voltage, chan->cur_dac_steps,
           chan->cur_current);
}