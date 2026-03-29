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
#include "ina219.h"
#include "common.h"

const char *_App_State_Lookup[] = {
    "Pre-Init",          "Init Controllers", "Test Controllers", "Start Master",
    "Check Temperature", "Check Power",      "Check Display",
};

void ping_peripherals(App_t *app);
void init_controllers(App_t *app);
void test_controllers(App_t *app);
void check_temp(App_t *app);
void check_power(App_t *app);
void check_display(App_t *app);

void app_error_check(App_t *app)
{
    if (app->error.code == ERROR_NONE) {
        return;
    }

    printf("\nENTERING DEBUG MODE\n**********\nApp State: %s\nController: %s\nPeripheral: "
           "%s\nFunction: %s\nError: %s\r\n\n",
           _App_State_Lookup[app->state], _Controller_Lookup[app->error.controller],
           _Peripheral_Lookup[app->error.peripheral], _Function_Lookup[app->error.function],
           _Error_Message_Lookup[app->error.code]);

    // _Error_Blink_t blink = _Error_Blink_Lookup[app->error.code];

    while (1) {
        // for (uint8_t i = 0; i < app->error.peripheral; i++) {
        //     LED_Controller_SetLED(&app->led_controller, LED_STATUS, LED_COLOR_RED);
        //     HAL_Delay(1000);
        //     LED_Controller_SetLED(&app->led_controller, LED_STATUS, LED_OFF);
        //     HAL_Delay(1000);
        // }
        // HAL_Delay(1500);
        // for (uint8_t i = 0; i < blink.blinks; i++) {
        //     LED_Controller_SetLED(&app->led_controller, LED_STATUS, LED_COLOR_RED);
        //     HAL_Delay(500);
        //     LED_Controller_SetLED(&app->led_controller, LED_STATUS, LED_OFF);
        //     HAL_Delay(500);
        // }
        // HAL_Delay(1500);
    }
}

void error_callback(void *ctx, _Error_t error)
{
    App_t *app = (App_t *)ctx;
    app->error.controller = error.controller;
    app->error.peripheral = error.peripheral;
    app->error.function = error.function;
    app->error.code = error.code;
    app_error_check(app);
}

void App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle)
{
    app->i2c_handle = i2c_handle;

    app->display_controller.error_cb = error_callback;
    app->display_controller.error_ctx = app;
    app->display_controller.i2c_handle = i2c_handle;

    app->led_controller.error_cb = error_callback;
    app->led_controller.error_ctx = app;

    app->power_controller.error_cb = error_callback;
    app->power_controller.error_ctx = app;
    app->power_controller.i2c_handle = i2c_handle;

    app->rotary_controller.error_cb = error_callback;
    app->rotary_controller.error_ctx = app;

    app->temperature_controller.error_cb = error_callback;
    app->temperature_controller.error_ctx = app;
    app->temperature_controller.i2c_handle = i2c_handle;

    app->state = APP_STATE_PING_PERIHPERALS;
    ping_peripherals(app);

    Power_Controller_ToggleBuck(&app->power_controller, POWER_BUCK_6V5);
    Power_Controller_ToggleBuck(&app->power_controller, POWER_BUCK_12V);
    Power_Controller_ToggleBuck(&app->power_controller, POWER_BUCK_VAR);

    app->state = APP_STATE_INIT_CONTROLLERS;
    init_controllers(app);

    // app->state = APP_STATE_TEST_CONTROLLERS;
    // test_controllers(app);

    // LED_Controller_SetLED(&app->led_controller, LED_STATUS, LED_COLOR_GREEN);

    app->state = APP_STATE_START_MASTER;
}

void ping_peripherals(App_t *app)
{
    Power_Controller_PingINA(&app->power_controller, INA_I2C_ADDRESS_MAIN, app->i2c_handle);
    Power_Controller_PingINA(&app->power_controller, INA_I2C_ADDRESS_6V5, app->i2c_handle);
    Power_Controller_PingINA(&app->power_controller, INA_I2C_ADDRESS_12V, app->i2c_handle);
    Power_Controller_PingINA(&app->power_controller, INA_I2C_ADDRESS_VAR, app->i2c_handle);

    Power_Controller_PingMCP(&app->power_controller, app->i2c_handle);

    Temperature_Controller_SensorPing(&app->temperature_controller, app->i2c_handle);
    // TODO: add second ping

    Display_Controller_PingGME(&app->display_controller, app->i2c_handle);
    // TODO: add second ping

    // if all peripherals respond, switch the bucks on

    HAL_Delay(100);
}

void init_controllers(App_t *app)
{
    Power_Controller_Init(&app->power_controller, app->i2c_handle);
    Temperature_Controller_Init(&app->temperature_controller, app->i2c_handle);
    Display_Controller_Init(&app->display_controller, app->i2c_handle);
}

void test_controllers(App_t *app)
{
    // test display, power, temperature, status controllers
}

void App_Run(App_t *app)
{
    switch (app->state) {

    case APP_STATE_PRE_INIT:
    case APP_STATE_INIT_CONTROLLERS:
    case APP_STATE_TEST_CONTROLLERS:
    case APP_STATE_START_MASTER:
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

void check_temp(App_t *app) { }

void check_power(App_t *app) { }