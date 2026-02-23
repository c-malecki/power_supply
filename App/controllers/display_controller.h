#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include "stm32f4xx_hal.h"

typedef struct
{

} Display_Controller_t;

uint8_t Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);

#endif // __DISPLAY_CONTROLLER_H__