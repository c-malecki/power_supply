#include "main.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "power_controller.h"

static Power_Controller_t pwr_ctrl;
void print_readings(Channel_VAR_t *chan);

App_Status_t App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle)
{
    App_Status_t status;
    status.ctrl = APP_CTRL;
    status.err = 0;

    app->i2c_handle = i2c_handle;

    status.err = Power_Controller_Init(&pwr_ctrl, app->i2c_handle);
    if (status.err != 0) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    app->pwr_ctrl = &pwr_ctrl;

    return status;
}

App_Status_t App_Dev_Test(App_t *app)
{
    App_Status_t status;
    status.ctrl = APP_CTRL;
    status.err = 0;

    status.err =
        Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, CHANNEL_VAR_MIN_VOLTAGE);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    Channel_VAR_EnableOutput(app->pwr_ctrl->chan_var, true);

    HAL_Delay(500);

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, 3.3f);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, 5.0f);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, 9.0f);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    print_readings(app->pwr_ctrl->chan_var);

    status.err = Channel_VAR_SetVoltage(app->pwr_ctrl->chan_var, app->i2c_handle, 12.0f);
    if (status.err != HAL_OK) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    print_readings(app->pwr_ctrl->chan_var);

    return status;
}

void print_readings(Channel_VAR_t *chan)
{
    printf("Channel_VAR:\nenabled %u\ntarget_v %.4fV\ncur_v %.4fV\ncur_dac_steps %u\ncur_i "
           "%.4fmA\r\n\n",
           chan->output_enabled, chan->target_voltage, chan->cur_voltage, chan->cur_dac_steps,
           chan->cur_current);
}