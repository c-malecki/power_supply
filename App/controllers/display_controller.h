#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
} Display_Controller_t;

_Error_Codes Display_Controller_PingPeripherals(I2C_HandleTypeDef *i2c_handle);
void Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Display_Controller_ShowVariableChannel(float voltage, float current);
void Display_Controller_State_Print(Display_Controller_t *ctrl);

#endif // __DISPLAY_CONTROLLER_H__