#ifndef __APP_H__
#define __APP_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
//
#include "display_controller.h"
#include "input_controller.h"
#include "led_controller.h"
#include "power_controller.h"
#include "temperature_controller.h"

typedef enum {
    APP_STATE_INIT_PRPH_PING = 0,
    APP_STATE_INIT_CTRL_INIT,
    APP_STATE_INIT_CTRL_TEST,
    APP_STATE_RUN_CHECK_TEMP,
    APP_STATE_RUN_CHECK_PWR,
} App_States;

typedef enum {
    APP_CTRL_OK = 0,
    APP_CTRL_DSP,
    APP_CTRL_PWR,
    APP_CTRL_TEMP,
} App_Controllers;

typedef enum {
    APP_PRPH_OK = 0,
    APP_PRPH_MCP,
    APP_PRPH_INA,
    APP_PRPH_GME,
    APP_PRPH_DS18
} App_Peripherals;

typedef enum {
    APP_ERR_OK = 0,
    APP_ERR_I2C_ERROR,
    APP_ERR_I2C_BUSY,
    APP_ERR_I2C_TIMEOUT,
    APP_ERR_PWR_OVERCURRENT,
    APP_ERR_PWR_OVERVOLTAGE
} App_Errors;

typedef struct
{
    App_States state;
    App_Controllers ctrl;
    App_Peripherals prph;
    App_Errors err;
} App_Status_t;

typedef struct
{
    Display_Controller_t dsp_ctrl;
    Input_Controller_t input_ctrl;
    LED_Controller_t led_ctrl;
    Power_Controller_t pwr_ctrl;
    Temperature_Controller_t temp_ctrl;
    I2C_HandleTypeDef *i2c_handle;
    App_States state;
    App_Status_t status;
    bool DEBUG_MODE_ON;
} App_t;

void App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle);
void App_Run(App_t *app);
void App_Status_Check(App_t *app);

#endif // __APP_H__