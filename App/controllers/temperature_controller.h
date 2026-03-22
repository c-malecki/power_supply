#ifndef __TEMPERATURE_CONTROLLER_H__
#define __TEMPERATURE_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define FAN_TEMP_OFF 27
#define FAN_TEMP_MIN 30
#define FAN_TEMP_MAX 50
#define FAN_PWM_MIN 300
#define FAN_PWM_MAX 1000

#define TEMP_CRITICAL 55

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    int16_t cur_temp;
    uint32_t fan_last;
    uint32_t fan_cur;
    uint32_t fan_diff;
    uint32_t last_tick;
    float fan_rpm;
    uint8_t fan_first;
    Error_Callback_t error_cb;
    void *error_ctx;
} Temperature_Controller_t;

void Temperature_Controller_PingSHT(Temperature_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Temperature_Controller_Init(Temperature_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Temperature_Controller_Read(Temperature_Controller_t *ctrl);
void Temperature_Controller_UpdateFan(Temperature_Controller_t *ctrl);

#endif // __TEMPERATURE_CONTROLLER_H__