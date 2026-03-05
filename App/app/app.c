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

void ping_peripherals(App_t *app);
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

    printf("Entering debug mode:\r\n\n");

    app->DEBUG_MODE_ON = true;
    App_Status_SetRGB(255, 0, 0);
    printf("Error:\n\nState: %s\nController: %s\nPeripheral: %s\n Status:%s\r\n\n",
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

void App_Status_SetRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, red);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, green);
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, blue);
}

void App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle)
{
    app->status.ctrl = APP_CTRL_OK;
    app->status.prph = APP_PRPH_OK;
    app->status.err = APP_ERR_OK;

    app->i2c_handle = i2c_handle;

    app->LED_MAIN_ON = true;

    // main debug LED
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // TIM1_CH1 G
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // TIM1_CH2 R
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // TIM1_CH3 B

    // 3V3 power channel LED
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3); // TIM5_CH3
    HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4); // TIM5_CH4
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // TIM2_CH3

    // 5V power channel LED
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // TIM3_CH1
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // TIM2_CH2
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // TIM2_CH1

    // variable power channel LED
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // TIM4_CH2
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); // TIM4_CH1
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2); // TIM3_CH2

    App_Status_SetRGB(0, 0, 255);

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
    test_controllers(app);
    if (app->status.err != APP_ERR_OK) {
        App_Status_Check(app);
    }

    app->state = APP_STATE_RUN_CHECK_TEMP;
}

void ping_peripherals(App_t *app)
{
    printf("ping_peripherals\r\n\n");

    app->status.err = Display_Controller_GME_Ping(app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_DSP;
        app->status.prph = APP_PRPH_GME;
        return;
    }

    app->status.err = Power_Controller_MCP_Ping(app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_PWR;
        app->status.prph = APP_PRPH_MCP;
        return;
    }

    app->status.err = Power_Controller_INA_Ping(app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_PWR;
        app->status.prph = APP_PRPH_INA;
        return;
    }

    // ds18 handle needs to be initialized first before ping since it doesn't
    // use I2C like the other hardware modules
    app->status.err = Temperature_Controller_Ping_And_Init(&temp_ctrl);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_TEMP;
        app->status.prph = APP_PRPH_DS18;
        return;
    }
}

void init_controllers(App_t *app)
{
    printf("init_controllers\r\n");

    app->status.err = Display_Controller_Init(&dsp_ctrl, app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_DSP;
        return;
    }
    app->dsp_ctrl = &dsp_ctrl;

    app->status.err = Power_Controller_Init(&pwr_ctrl, app->i2c_handle);
    if (app->status.err != APP_ERR_OK) {
        app->status.ctrl = APP_CTRL_PWR;
        return;
    }
    app->pwr_ctrl = &pwr_ctrl;

    app->temp_ctrl = &temp_ctrl;
}

void test_controllers(App_t *app)
{
    printf("test_controllers\r\n");
    // test display, power, temperature, status controllers

    // uint8_t err = Channel_VAR_UpdateValues(app->pwr_ctrl->chan_var, app->i2c_handle);
    // if (err != HAL_OK) {
    //     return err;
    // }
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