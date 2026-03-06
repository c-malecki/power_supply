#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include "stm32f4xx_hal.h"

typedef enum {
    DISPLAY_MENU = 0,
    DISPLAY_CHAN
} Display_Modes;

typedef struct
{

} Display_Chan_t;

typedef struct
{

} Display_Menu_t;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
} Display_Controller_t;

uint8_t Display_Controller_GME_Ping(I2C_HandleTypeDef *i2c_handle);
uint8_t Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Display_Controller_State_Print(Display_Controller_t *ctrl);

#endif // __DISPLAY_CONTROLLER_H__