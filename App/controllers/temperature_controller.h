#ifndef __TEMPERATURE_CONTROLLER_H__
#define __TEMPERATURE_CONTROLLER_H__

#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "ds18b20.h"
#include "stm32f4xx_hal.h"

#define TEMP_MAX 50
#define TEMP_CRITICAL 60

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef enum {
    TEMP_STATE_INIT = 0,
    TEMP_STATE_READY,
    TEMP_STATE_CNV,
    TEMP_STATE_REQ_READ,
    TEMP_STATE_READ_C
} Temperature_Controller_States;

typedef struct
{
    Temperature_Controller_States prev_state;
    Temperature_Controller_States cur_state;
    volatile int16_t cur_temp;
    ds18b20_t *ds18;
    Error_Callback_t error_cb;
    void *error_ctx;
} Temperature_Controller_t;

void Temperature_Controller_PingAndInit(Temperature_Controller_t *ctrl);
void Temperature_Controller_StartCnv(Temperature_Controller_t *ctrl);
void Temperature_Controller_ReqRead(Temperature_Controller_t *ctrl);
void Temperature_Controller_EndRead(Temperature_Controller_t *ctrl);

void Temperature_Controller_TimCB(TIM_HandleTypeDef *htim);
bool Temperature_Controller_CheckReady(Temperature_Controller_t *ctrl);

#endif // __TEMPERATURE_CONTROLLER_H__