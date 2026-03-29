#ifndef __APP_H__
#define __APP_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
//
#include "common.h"
#include "display_controller.h"
#include "led_controller.h"
#include "power_controller.h"
#include "rotary_controller.h"
#include "temperature_controller.h"

typedef enum {
    APP_STATE_PRE_INIT = 0,
    APP_STATE_PING_PERIHPERALS,
    APP_STATE_INIT_CONTROLLERS,
    APP_STATE_TEST_CONTROLLERS,
    APP_STATE_START_MASTER,
    APP_STATE_CHECK_TEMPERATURE,
    APP_STATE_CHECK_POWER,
    APP_STATE_CHECK_DISPLAY
} App_States;

extern const char *_App_State_Lookup[];

typedef struct
{
    Display_Controller_t display_controller;
    Rotary_Controller_t rotary_controller;
    LED_Controller_t led_controller;
    Power_Controller_t power_controller;
    Temperature_Controller_t temperature_controller;
    I2C_HandleTypeDef *i2c_handle;
    App_States state;
    _Error_t error;
} App_t;

void App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle);
void App_Run(App_t *app);
void App_ErrorCheck(App_t *app);

#endif // __APP_H__