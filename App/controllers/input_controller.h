#ifndef __INPUT_CONTROLLER_H__
#define __INPUT_CONTROLLER_H__

#include "stm32f4xx_hal.h"

typedef enum {
    DISPLAY_VAR_CHAN = 0,
    DISPLAY_MENU_OPTS
} Input_Rotary_Modes;

typedef enum {
    OFF = 0,
    ON
} Input_Button_States;

typedef struct
{
    uint8_t last_clk;
    bool pressed;
    Input_Rotary_Modes mode;
    uint16_t clk_pin;
    uint16_t dt_pin;
    uint16_t sw_pin;
    int32_t position;
    GPIO_TypeDef *clk_port;
    GPIO_TypeDef *dt_port;
    GPIO_TypeDef *sw_port;
} Input_Rotary_t;

typedef struct
{
    Input_Button_States chan_3v3;
    Input_Button_States chan_5v;
    Input_Button_States chan_var;
    Input_Rotary_t rotary;
} Input_Controller_t;

void Input_Controller_State_Print(Input_Controller_t *ctrl);

#endif // __INPUT_CONTROLLER_H__