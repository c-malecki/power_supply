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
    "Init",
    "Check Power",
    "Check Temperature",
    "Check Display",
};

void init_controllers(App_t *app, I2C_HandleTypeDef *i2c_handle);
void ping_peripherals(App_t *app);
void power_bucks(App_t *app);
void run_controllers(App_t *app);
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
    app->display_controller.error_cb = error_callback;
    app->display_controller.error_ctx = app;

    app->led_controller.error_cb = error_callback;
    app->led_controller.error_ctx = app;

    app->power_controller.error_cb = error_callback;
    app->power_controller.error_ctx = app;

    app->rotary_controller.error_cb = error_callback;
    app->rotary_controller.error_ctx = app;

    app->temperature_controller.error_cb = error_callback;
    app->temperature_controller.error_ctx = app;

    init_controllers(app, i2c_handle);
    ping_peripherals(app);
    power_bucks(app);
    run_controllers(app);
    test_controllers(app);
}

void App_Run(App_t *app)
{
    switch (app->state) {

    case APP_STATE_INIT:
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

void init_controllers(App_t *app, I2C_HandleTypeDef *i2c_handle)
{
    Pwr_Ctrl_Init(&app->power_controller, i2c_handle);
    Temp_Ctrl_Init(&app->temperature_controller, i2c_handle);
    Dsp_Ctrl_Init(&app->display_controller, i2c_handle);
}

void ping_peripherals(App_t *app)
{
    Pwr_Ctrl_Ping(&app->power_controller);
    Temp_Ctrl_Ping(&app->temperature_controller);
    Dsp_Ctrl_Ping(&app->display_controller);
}

void power_bucks(App_t *app)
{
    Pwr_Buck_Toggle(&app->power_controller, PWR_BUCK_6V5);
    Pwr_Buck_Toggle(&app->power_controller, PWR_BUCK_12V);
    Pwr_Buck_Toggle(&app->power_controller, PWR_BUCK_VARV);
    HAL_Delay(100);
}

void run_controllers(App_t *app)
{
    Pwr_Ctrl_Run(&app->power_controller);
    Temp_Ctrl_Run(&app->temperature_controller);
    Dsp_Ctrl_Run(&app->display_controller);
}

void test_controllers(App_t *app)
{
    // test display, power, temperature, status controllers
}

void check_power(App_t *app)
{
    for (uint8_t i = 0; i < PWR_CHAN_COUNT; i++) {
        Pwr_Chan_t *chan = &app->power_controller.channels[i];
        Pwr_Chan idx = (Pwr_Chan)i;

        if (chan->toggle_pending == true) {
            Pwr_Chan_Toggle(&app->power_controller, idx);
        }
    }

    for (uint32_t i = 0; i < PWR_BUCK_COUNT; i++) {
        Pwr_Buck_t *buck = &app->power_controller.bucks[i];

        if ((HAL_GetTick() - buck->ina.last_read) >= INA_READ_INTERVAL) {
            INA_Read(&buck->ina, app->power_controller.i2c_handle);
        }
    }
}

void check_temp(App_t *app)
{
    for (uint8_t i = 0; i < TEMP_SENSOR_COUNT; i++) {
        Temp_Sensor idx = (Temp_Sensor)i;
        Temp_Sensor_t *sensor = &app->temperature_controller.sensors[idx];

        switch (sensor->state) {
        case TEMP_SENSOR_STATE_INIT:
        case TEMP_SENSOR_STATE_START_READ:
            break;

        case TEMP_SENSOR_STATE_READY:
            Temp_Sensor_StartRead(&app->temperature_controller, idx);
            break;

        case TEMP_SENSOR_STATE_WAIT_RESULT:
            Temp_Sensor_CheckResult(&app->temperature_controller, idx);
            break;

        case TEMP_SENSOR_STATE_GET_RESULT:
            Temp_Sensor_GetResult(&app->temperature_controller, idx);
            break;

        case TEMP_SENSOR_STATE_WAIT_READ:
            Temp_Sensor_CheckReady(&app->temperature_controller, idx);
            break;
        }
    }
}