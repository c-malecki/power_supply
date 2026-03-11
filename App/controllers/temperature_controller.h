#ifndef __TEMPERATURE_CONTROLLER_H__
#define __TEMPERATURE_CONTROLLER_H__

#include "ds18b20.h"
#include <stdint.h>
#include "common.h"

#define TEMP_MAX 50
#define TEMP_CRITICAL 60

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef enum {
    TEMP_STATE_IDLE,
    TEMP_STATE_START_CONVERSION,
    TEMP_STATE_WAIT_CONVERSTION,
    TEMP_STATE_START_READ,
    TEMP_STATE_WAIT_READ,
    TEMP_STATE_CHECK_TRESHOLDS,
} Temperature_Controller_States;

typedef struct
{
    Temperature_Controller_States state;
    volatile int16_t cur_temp;
    ds18b20_t ds18;
    Error_Callback_t error_cb;
    void *error_ctx;
} Temperature_Controller_t;

void Temperature_Controller_Ping_And_Init(Temperature_Controller_t *ctrl);
void Temperature_Controller_Read(Temperature_Controller_t *ctrl);
void Temperature_Controller_State_Print(Temperature_Controller_t *ctrl);

#endif // __TEMPERATURE_CONTROLLER_H__