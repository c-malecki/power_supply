#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "power_controller.h"

static Power_Controller_t pwr_ctrl;

App_Status_t App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle)
{
    App_Status_t status;
    status.ctrl = APP_CTRL;
    status.err = 0;

    app->i2c_handle = i2c_handle;

    // ping all peripherals

    status.err = Power_Controller_Init(&pwr_ctrl, app->i2c_handle);
    if (status.err != 0) {
        status.ctrl = PWR_CTRL;
        return status;
    }

    app->pwr_ctrl = &pwr_ctrl;

    // run power tests

    return status;
}

uint8_t startup_tests(App_t *app)
{
    uint8_t err = Channel_VAR_UpdateValues(app->pwr_ctrl->chan_var, app->i2c_handle);
    if (err != HAL_OK) {
        return err;
    }
}

// Startup test
// ping all peripherals (MCP, INA, OLED)
// start INA reading and verify 0V 0I
// open gate to buck
// run tests to check levels