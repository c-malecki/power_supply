#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>
#include <stdio.h>
//
#include "app.h"
#include "display_controller.h"
#include "power_controller.h"
#include "temperature_controller.h"
#include "INA219.h"
#include "GME12864-13.h"

static Display_Controller_t dsp_ctrl;
static Power_Controller_t pwr_ctrl;
static Temperature_Controller_t temp_ctrl;

static char *app_states[5] = { "Ping Prphs", "Init Ctrls", "Test Ctrls", "Check Temp",
                               "Check Pwr" };
static char *app_ctrls[4] = { "APP", "DSP", "PWR", "TEMP" };
static char *app_prphs[4] = { "", "MCP", "INA", "GME" };
static char *app_errs[6] = { "OK",          "I2C Error",   "I2C Busy",
                             "I2C Timeout", "Overcurrent", "Overvoltage" };

void ping_peripherals(App_t *app, App_Status_t *status);
void init_controllers(App_t *app, App_Status_t *status);
void test_controllers(App_t *app, App_Status_t *status);
void check_temp(App_t *app);
void check_power(App_t *app);

void App_Status_Check(App_t *app, App_Status_t *status)
{
    if (status->err == 0) {
        app->DEBUG_MODE_ON = false;
        return;
    }

    printf("Entering debug mode:\r\n\n");

    app->DEBUG_MODE_ON = true;
    App_Status_SetRGB(255, 0, 0);
    printf("Error:\n\nState: %s\nController: %s\nPeripheral: %s\n Status:%s\r\n\n",
           app_states[app->state], app_ctrls[status->ctrl], app_prphs[status->prph],
           app_errs[status->err]);

    // uint32_t last = 0;
    // int count = 0;

    while (1) {
        // TODO: Blink LED_MAIN with error code
        // if ((HAL_GetTick() - last) > 500) {
        //     last = HAL_GetTick();
        // }
    }
}

void App_Status_SetRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    __HAL_TIM_SET_COMPARE(&htim1, LED_RED_TIMCH, red);
    __HAL_TIM_SET_COMPARE(&htim1, LED_GREEN_TIMCH, green);
    __HAL_TIM_SET_COMPARE(&htim1, LED_BLUE_TIMCH, blue);
}

void App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle)
{
    App_Status_t status;
    status.ctrl = APP_CTRL_OK;
    status.err = APP_ERR_OK;

    app->i2c_handle = i2c_handle;

    app->LED_MAIN_ON = true;
    HAL_TIM_PWM_Start(&htim1, LED_RED_TIMCH);
    HAL_TIM_PWM_Start(&htim1, LED_GREEN_TIMCH);
    HAL_TIM_PWM_Start(&htim1, LED_BLUE_TIMCH);
    App_Status_SetRGB(0, 255, 0);

    app->state = APP_STATE_INIT_PRPH_PING;
    ping_peripherals(app, &status);
    if (status.err != APP_ERR_OK) {
        App_Status_Check(app, &status);
    }

    app->state = APP_STATE_INIT_CTRL_INIT;
    init_controllers(app, &status);
    if (status.err != APP_ERR_OK) {
        App_Status_Check(app, &status);
    }

    app->state = APP_STATE_INIT_CTRL_TEST;
    test_controllers(app, &status);
    if (status.err != APP_ERR_OK) {
        App_Status_Check(app, &status);
    }

    app->state = APP_STATE_RUN_CHECK_TEMP;
}

void App_Run(App_t *app)
{
    switch (app->state) {

    case APP_STATE_RUN_CHECK_TEMP:
    {
        check_temp(app);
    }

    case APP_STATE_RUN_CHECK_PWR:
    {
        check_power(app);
    }
    }
}

void check_temp(App_t *app) { }

void check_power(App_t *app)
{
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
}

void ping_peripherals(App_t *app, App_Status_t *status)
{
    status->err = (uint8_t)HAL_I2C_IsDeviceReady(app->i2c_handle, GME_I2C_ADDR, 3, 100);
    if (status->err != APP_ERR_OK) {
        status->ctrl = APP_CTRL_DSP;
        status->prph = APP_PRPH_GME;
        return;
    }

    status->err = (uint8_t)HAL_I2C_IsDeviceReady(app->i2c_handle, MCP_I2C_ADDR, 3, 100);
    if (status->err != APP_ERR_OK) {
        status->ctrl = APP_CTRL_PWR;
        status->prph = APP_PRPH_MCP;
        return;
    }

    status->err = (uint8_t)HAL_I2C_IsDeviceReady(app->i2c_handle, INA_I2C_ADDR, 3, 100);
    if (status->err != APP_ERR_OK) {
        status->ctrl = APP_CTRL_PWR;
        status->prph = APP_PRPH_INA;
        return;
    }
}

void init_controllers(App_t *app, App_Status_t *status)
{
    status->err = Display_Controller_Init(&dsp_ctrl, app->i2c_handle);
    if (status->err != APP_ERR_OK) {
        status->ctrl = APP_CTRL_DSP;
        return;
    }
    app->dsp_ctrl = &dsp_ctrl;

    status->err = Power_Controller_Init(&pwr_ctrl, app->i2c_handle);
    if (status->err != APP_ERR_OK) {
        status->ctrl = APP_CTRL_PWR;
        return;
    }
    app->pwr_ctrl = &pwr_ctrl;

    status->err = Temperature_Controller_Init(&temp_ctrl);
    if (status->err != APP_ERR_OK) {
        status->ctrl = APP_CTRL_TEMP;
        return;
    }
    app->temp_ctrl = &temp_ctrl;
}

void test_controllers(App_t *app, App_Status_t *status)
{
    // test display, power, temperature, status controllers

    // uint8_t err = Channel_VAR_UpdateValues(app->pwr_ctrl->chan_var, app->i2c_handle);
    // if (err != HAL_OK) {
    //     return err;
    // }
}