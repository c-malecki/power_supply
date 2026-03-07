#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
#include "i2c.h"
//
#include "app.h"
#include "display_controller.h"
#include "rotary_controller.h"
#include "led_controller.h"
#include "power_controller.h"
#include "temperature_controller.h"
#include "INA219.h"
#include "common.h"

const char *_App_State_Lookup[] = {
    "Init: Ping Peripherals", "Init: Init Controllers", "Init: Test Controllers",
    "Run: Check Temperature", "Run: Check Power",
};

void ping_peripherals(App_t *app);
void relay_enable_power(void);
void init_controllers(App_t *app);
void test_controllers(App_t *app);
void check_temp(App_t *app);
void check_power(App_t *app);
void check_display(App_t *app);

void App_Status_Check(App_t *app)
{
    if (app->status.error_code == ERROR_NONE) {
        app->DEBUG_MODE_ON = false;
        return;
    }

    printf("\nENTERING DEBUG MODE\n**********\r\n");

    app->DEBUG_MODE_ON = true;
    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_RED);

    printf("Error:\nState: %s\nController: %s\nPeripheral: %s\nStatus:%s\r\n\n",
           _App_State_Lookup[app->state], _Controller_Lookup[app->status.controller],
           _Peripheral_Lookup[app->status.peripheral],
           _Error_Message_Lookup[app->status.error_code]);

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

    app->status.controller = CONTROLLER_NONE;
    app->status.peripheral = PERIPHERAL_NONE;
    app->status.error_code = ERROR_NONE;

    app->i2c_handle = i2c_handle;

    LED_Controller_Init(&app->led_ctrl);
    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_YELLOW);

    app->state = APP_STATE_PING_PERIPHERALS;
    ping_peripherals(app);
    if (app->status.error_code != ERROR_NONE) {
        App_Status_Check(app);
    }

    // if all peripherals respond, switch relay to let power to the output channels
    HAL_GPIO_WritePin(RELAY_CHAN_PWR_GPIO_Port, RELAY_CHAN_PWR_Pin, GPIO_PIN_SET);

    app->state = APP_STATE_INIT_CONTROLLERS;
    init_controllers(app);
    if (app->status.error_code != ERROR_NONE) {
        App_Status_Check(app);
    }

    app->state = APP_STATE_INIT_TEST_CONTROLLERS;
    // test_controllers(app);
    // if (app->status.error_code != ERROR_NONE) {
    //     App_Status_Check(app);
    // }

    app->state = APP_STATE_CHECK_TEMPERATURE;
}

void ping_peripherals(App_t *app)
{
    printf("ping_peripherals\n**********\r\n");

    Power_Controller_Ping_Result_t pwr_res = Power_Controller_PingPeripherals(app->i2c_handle);
    if (pwr_res.mcp != ERROR_NONE) {
        app->status.error_code = pwr_res.mcp;
        app->status.controller = CONTROLLER_POWER;
        app->status.peripheral = PERIPHERAL_MCP;
        return;
    }
    if (pwr_res.ina != ERROR_NONE) {
        app->status.error_code = pwr_res.ina;
        app->status.controller = CONTROLLER_POWER;
        app->status.peripheral = PERIPHERAL_INA;
        return;
    }
    printf("power: OK\r\n");

    app->status.error_code = Display_Controller_PingPeripherals(app->i2c_handle);
    if (app->status.error_code != ERROR_NONE) {
        app->status.controller = CONTROLLER_DISPLAY;
        app->status.peripheral = PERIPHERAL_GME;
        return;
    }
    printf("display: OK\r\n");

    // ds18 handle needs to be initialized first before ping since it doesn't
    // use I2C like the other hardware modules
    app->status.error_code = Temperature_Controller_Ping_And_Init(&app->temp_ctrl);
    if (app->status.error_code != ERROR_NONE) {
        app->status.controller = CONTROLLER_TEMPERATURE;
        app->status.peripheral = PERIPHERAL_DS18;
        return;
    }
    printf("temperature: OK\r\n\n");
}

void init_controllers(App_t *app)
{
    printf("init_controllers\n**********\r\n");

    app->status.error_code = Power_Controller_Init(&app->pwr_ctrl, app->i2c_handle);
    if (app->status.error_code != ERROR_NONE) {
        app->status.controller = CONTROLLER_POWER;
        return;
    }
    printf("power: OK\r\n");

    // temp is initialized in the ping test
    printf("temperature: OK\r\n");

    // Display_Controller_Init(&app->dsp_ctrl, app->i2c_handle);
    printf("display: OK\r\n\n");
}

void test_controllers(App_t *app)
{
    printf("test_controllers\n**********\r\n");
    // test display, power, temperature, status controllers
}

void App_Run(App_t *app)
{
    switch (app->state) {

    case APP_STATE_PING_PERIPHERALS:
    case APP_STATE_INIT_CONTROLLERS:
    case APP_STATE_INIT_TEST_CONTROLLERS:
        break;

    case APP_STATE_CHECK_TEMPERATURE:
        check_temp(app);
        break;

    case APP_STATE_CHECK_POWER:
        check_power(app);
        break;

    case APP_STATE_CHECK_DISPLAY:
        check_display(app);
        break;
    }
}

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