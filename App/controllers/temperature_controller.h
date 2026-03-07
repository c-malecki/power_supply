#ifndef __TEMPERATURE_CONTROLLER_H__
#define __TEMPERATURE_CONTROLLER_H__

#include "ds18b20.h"
#include <stdint.h>
#include "common.h"

#define TEMP_MAX 50
#define TEMP_CRITICAL 60

typedef struct
{
    ds18b20_t ds18;
    volatile int16_t cur_temp;
} Temperature_Controller_t;

_Error_Codes Temperature_Controller_Ping_And_Init(Temperature_Controller_t *ctrl);
_Error_Codes Temperature_Controller_Read(Temperature_Controller_t *ctrl);
void Temperature_Controller_State_Print(Temperature_Controller_t *ctrl);

#endif // __TEMPERATURE_CONTROLLER_H__