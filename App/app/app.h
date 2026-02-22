#ifndef __APP_H__
#define __APP_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
//
#include "power_controller.h"

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Power_Controller_t *pwr_ctrl;
} App_t;

typedef enum {
    APP_CTRL = 0,
    DSP_CTRL,
    PWR_CTRL,
    TMP_CTRL,
} App_Controllers_t;

typedef struct
{
    App_Controllers_t ctrl;
    uint8_t err;
} App_Status_t;

App_Status_t App_Init(App_t *app, I2C_HandleTypeDef *i2c_handle);
App_Status_t App_Dev_Test(App_t *app);

#endif