#ifndef __TEMPERATURE_CONTROLLER_H__
#define __TEMPERATURE_CONTROLLER_H__

#include "ds18b20.h"
#include <stdint.h>

#define TEMP_MAX 50
#define TEMP_CRITICAL 60

typedef struct
{
    ds18b20_t *ds18;
    volatile int16_t cur_temp;
} Temperature_Controller_t;

uint8_t Temperature_Controller_Init(Temperature_Controller_t *ctrl);
uint8_t Temperature_Controller_Read(Temperature_Controller_t *ctrl);
uint8_t Temperature_Controller_Ping(Temperature_Controller_t *ctrl);

#endif // __TEMPERATURE_CONTROLLER_H__