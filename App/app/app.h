#ifndef __APP_H__
#define __APP_H__

#include "stm32f4xx_hal.h"
#include "chan.h"
#include <stdint.h>

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    PWR_Chan_t *pwr_chans[3];
} APP_t;

typedef enum {
    APP_CTRL = 0,
    PWR_CTRL,
    DSP_CTRL
} APP_Ctrl_t;

typedef struct
{
    APP_Ctrl_t ctrl;
    uint8_t err;
} APP_Status_t;

APP_Status_t App_Init(APP_t *app, I2C_HandleTypeDef *i2c_handle);
APP_Status_t App_Test(APP_t *app);

#endif