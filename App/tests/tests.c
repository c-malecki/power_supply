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

void Test_MainRBG(App_t *app)
{
    App_Status_t status;
    status.ctrl = APP_CTRL_DSP;
    status.prph = APP_PRPH_GME;
    status.err = APP_ERR_I2C_TIMEOUT;

    HAL_Delay(1000);

    App_Status_Check(app, &status);

    HAL_Delay(1000);

    App_Status_SetRGB(0, 255, 0);
}

void Test_ChannelVar(App_t *app)
{
    App_Status_t status;
    status.ctrl = APP_CTRL_PWR;
    status.prph = APP_PRPH_OK;
    status.err = APP_ERR_OK;

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MIN_VOLTAGE);
    App_Status_Check(app, &status);

    Channel_VAR_EnableOutput(app->pwr_ctrl->chan_var, true);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MAX_VOLTAGE);
    App_Status_Check(app, &status);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MIN_VOLTAGE);
    App_Status_Check(app, &status);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MAX_VOLTAGE);
    App_Status_Check(app, &status);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, MCP_MIN_VOLTAGE);
    App_Status_Check(app, &status);

    print_readings(app->pwr_ctrl->chan_var);

    Channel_VAR_EnableOutput(app->pwr_ctrl->chan_var, false);

    print_readings(app->pwr_ctrl->chan_var);
}

void print_readings(Channel_VAR_t *chan)
{
    printf("Channel_VAR:\nenabled %u\ntarget_v %.4fV\ncur_v %.4fV\ncur_dac_steps %u\ncur_i "
           "%.4fmA\r\n\n",
           chan->output_enabled, chan->target_voltage, chan->cur_voltage, chan->cur_dac_steps,
           chan->cur_current);
}