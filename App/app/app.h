#ifndef __APP_H__
#define __APP_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
//
#include "display_controller.h"
#include "led_controller.h"
#include "power_controller.h"
#include "rotary_controller.h"
#include "temperature_controller.h"
#include "common.h"

typedef enum {
    APP_STATE_PING_PERIPHERALS = 0,
    APP_STATE_INIT_CONTROLLERS,
    APP_STATE_INIT_TEST_CONTROLLERS,
    APP_STATE_CHECK_TEMPERATURE,
    APP_STATE_CHECK_POWER,
    APP_STATE_CHECK_DISPLAY
} App_States;

extern const char *_App_State_Lookup[];

typedef struct
{
    _Controllers controller;
    _Peripherals peripheral;
    _Error_Codes error_code;
} App_Status_t;

typedef struct
{
    Display_Controller_t dsp_ctrl;
    Rotary_Controller_t rotary_ctrl;
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