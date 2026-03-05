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
    GPIO_TypeDef *clk_port;
    GPIO_TypeDef *dt_port;
    GPIO_TypeDef *sw_port;
    uint16_t clk_pin;
    uint16_t dt_pin;
    uint16_t sw_pin;
    uint8_t last_clk;
    int32_t position;
    bool pressed;
    Display_Modes mode;
} Display_Rotary_t;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Display_Rotary_t rotary;
} Display_Controller_t;

uint8_t Display_Controller_GME_Ping(I2C_HandleTypeDef *i2c_handle);
uint8_t Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);

#endif // __DISPLAY_CONTROLLER_H__