#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "display_controller.h"
#include "input_controller.h"
#include "led_controller.h"
#include "power_controller.h"
#include "temperature_controller.h"
#include "INA219.h"
#include "GME12864-13.h"

static char *app_states[5] = { "Ping Prphs", "Init Ctrls", "Test Ctrls", "Check Temp",
                               "Check Pwr" };
static char *app_ctrls[4] = { "APP", "DSP", "PWR", "TEMP" };
static char *app_prphs[4] = { "", "MCP", "INA", "GME" };
static char *app_errs[6] = { "OK",          "I2C Error",   "I2C Busy",
                             "I2C Timeout", "Overcurrent", "Overvoltage" };

void ping_peripherals(App_t *app);
void relay_enable_power(void);
void init_controllers(App_t *app);
void test_controllers(App_t *app);
void check_temp(App_t *app);
void check_power(App_t *app);

void App_Status_Check(App_t *app)
{
    if (app->status.err == 0) {
        app->DEBUG_MODE_ON = false;
        return;
    }

    printf("***ENTERING DEBUG MODE***:\r\n\n");

    app->DEBUG_MODE_ON = true;
    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_RED);

    printf("Error:\n\nState: %s\nController: %s\nPeripheral: %s\nStatus:%s\r\n\n",
           app_states[app->state], app_ctrls[app->status.ctrl], app_prphs[app->status.prph],
           app_errs[app->status.err]);

    // uint32_t last = 0;
    // int count = 0;

    while (1) {
        // TODO: Blink LED_MAIN with error code
        // if ((HAL_GetTick() - last) > 500) {
        //     last = HAL_GetTick();
        // }
    }
}

void App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle)
{
    printf("APP INIT\n**********\r\n");

    app->status.ctrl = APP_CTRL_OK;
    app->status.prph = APP_PRPH_OK;
    app->status.err = APP_ERR_OK;

    app->i2c_handle = i2c_handle;

    LED_Controller_Init(&app->led_ctrl);
    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_YELLOW);

    app->state = APP_STATE_INIT_PRPH_PING;
    ping_peripherals(app);
    if (app->status.err != APP_ERR_OK) {
        App_Status_Check(app);
    }

    // if all peripherals respond, switch relay to let power to the output channels
    HAL_GPIO_WritePin(RELAY_CHAN_PWR_GPIO_Port, RELAY_CHAN_PWR_Pin, GPIO_PIN_SET);

    app->state = APP_STATE_INIT_CTRL_INIT;
    init_controllers(app);
    if (app->status.err != APP_ERR_OK) {
        App_Status_Check(app);
    }

    app->state = APP_STATE_INIT_CTRL_TEST;
    // test_controllers(app);
    // if (app->status.err != APP_ERR_OK) {
    //     App_Status_Check(app);
    // }

    app->state = APP_STATE_RUN_CHECK_TEMP;
}

void ping_peripherals(App_t *app)
{
    printf("ping_peripherals\n**********\r\n");
    app->status.err = Display_Controller_PingPeripherals(app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_DSP;
        app->status.prph = APP_PRPH_GME;
        return;
    }
    printf("display: OK\r\n");

    Power_Controller_Ping_Result_t pwr_res = Power_Controller_PingPeripherals(app->i2c_handle);
    if (pwr_res.mcp != 0) {
        app->status.ctrl = APP_CTRL_PWR;
        app->status.prph = APP_PRPH_MCP;
        return;
    }
    if (pwr_res.ina != 0) {
        app->status.ctrl = APP_CTRL_PWR;
        app->status.prph = APP_PRPH_INA;
        return;
    }
    printf("power: OK\r\n");

    // ds18 handle needs to be initialized first before ping since it doesn't
    // use I2C like the other hardware modules
    app->status.err = Temperature_Controller_Ping_And_Init(&app->temp_ctrl);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_TEMP;
        app->status.prph = APP_PRPH_DS18;
        return;
    }
    printf("temperature: OK\r\n\n");
}

void init_controllers(App_t *app)
{
    printf("init_controllers\n**********\r\n");
    app->status.err = Display_Controller_Init(&app->dsp_ctrl, app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_DSP;
        return;
    }
    printf("display: OK\r\n");

    app->status.err = Power_Controller_Init(&app->pwr_ctrl, app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_PWR;
        return;
    }
    printf("power: OK\r\n");

    // temp is initialized in the ping test
    printf("temperature: OK\r\n\n");
}

void test_controllers(App_t *app)
{
    printf("test_controllers\n**********\r\n");
    // test display, power, temperature, status controllers
}

// void App_Run(App_t *app)
// {
//     switch (app->state) {

//     case APP_STATE_RUN_CHECK_TEMP:
//     {
//         check_temp(app);
//     }

//     case APP_STATE_RUN_CHECK_PWR:
//     {
//         check_power(app);
//     }
//     }
// }

// void check_temp(App_t *app) { }

// void check_power(App_t *app)
// {
// if (app->pwr_ctrl->chan_3v3->output_enabled == 1) {
//     if (!app->LED_3V3_ON)
//         HAL_GPIO_WritePin(app->pwr_ctrl->chan_3v3->led_port,
//         app->pwr_ctrl->chan_3v3->led_pin,
//                           GPIO_PIN_SET);
// } else {
//     HAL_GPIO_WritePin(app->pwr_ctrl->chan_3v3->led_port, app->pwr_ctrl->chan_3v3->led_pin,
//                       GPIO_PIN_RESET);
// }

// if (app->pwr_ctrl->chan_5v->output_enabled == 1) {
//     if (!app->LED_5V_ON)
//         HAL_GPIO_WritePin(app->pwr_ctrl->chan_5v->led_port, app->pwr_ctrl->chan_5v->led_pin,
//                           GPIO_PIN_SET);
// } else {
//     HAL_GPIO_WritePin(app->pwr_ctrl->chan_5v->led_port, app->pwr_ctrl->chan_5v->led_pin,
//                       GPIO_PIN_RESET);
// }

// if (app->pwr_ctrl->chan_var->output_enabled == 1) {
//     if (!app->LED_VAR_ON)
//         HAL_GPIO_WritePin(app->pwr_ctrl->chan_var->led_port,
//         app->pwr_ctrl->chan_var->led_pin,
//                           GPIO_PIN_SET);
// } else {
//     HAL_GPIO_WritePin(app->pwr_ctrl->chan_var->led_port, app->pwr_ctrl->chan_var->led_pin,
//                       GPIO_PIN_RESET);
// }
// }