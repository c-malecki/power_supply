#ifndef __ROTARY_CONTROLLER_H__
#define __ROTARY_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>

typedef enum {
    DISPLAY_VARIABLE_CHANNEL = 0,
    DISPLAY_MENU_OPTIONS
} Rotary_Modes;

typedef struct
{
    uint8_t last_clk;
    bool pressed;
    Rotary_Modes mode;
    uint16_t clk_pin;
    uint16_t dt_pin;
    uint16_t sw_pin;
    int32_t position;
    GPIO_TypeDef *clk_port;
    GPIO_TypeDef *dt_port;
    GPIO_TypeDef *sw_port;
} Rotary_t;

typedef struct
{
    Rotary_t rotary;
} Rotary_Controller_t;

void Rotary_Controller_State_Print(Rotary_Controller_t *ctrl);

#endif // __ROTARY_CONTROLLER_H__