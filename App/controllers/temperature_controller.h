#ifndef __TEMPERATURE_CONTROLLER_H__
#define __TEMPERATURE_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define TEMP_MAX 50
#define TEMP_CRITICAL 60

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    int16_t cur_temp;
    Error_Callback_t error_cb;
    void *error_ctx;
} Temperature_Controller_t;

void Temperature_Controller_PingSHT(Temperature_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Temperature_Controller_Init(Temperature_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Temperature_Controller_Read(Temperature_Controller_t *ctrl);

#endif // __TEMPERATURE_CONTROLLER_H__