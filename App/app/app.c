#include "main.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "power_controller.h"

Power_Controller_t pwr_ctrl;

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

    return status;
}