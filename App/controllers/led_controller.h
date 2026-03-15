#ifndef __LED_CONTROLLER_H__
#define __LED_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} Input_LED_Color_t;

extern const Input_LED_Color_t LED_COLOR_RED;
extern const Input_LED_Color_t LED_COLOR_GREEN;
extern const Input_LED_Color_t LED_COLOR_BLUE;
extern const Input_LED_Color_t LED_COLOR_YELLOW;
extern const Input_LED_Color_t LED_OFF;

/*
    main debug led
        red TIM2_CH2
        green TIM2_CH3
        blue TIM2_CH4

    variable power channel LED
        red TIM4_CH2
        green TIM4_CH1
        blue TIM3_CH2
*/

typedef struct
{
    uint32_t r_timch;
    uint32_t g_timch;
    uint32_t b_timch;
    TIM_HandleTypeDef *r_htim;
    TIM_HandleTypeDef *g_htim;
    TIM_HandleTypeDef *b_htim;
} Input_LED_t;

typedef enum {
    LED_STATUS = 0,
    LED_3V3,
    LED_5V,
    LED_VAR,
    LED_COUNT
} Input_LEDs;

typedef struct
{
    Input_LED_t leds[4];
    Error_Callback_t error_cb;
    void *error_ctx;
} LED_Controller_t;

void LED_Controller_Init(LED_Controller_t *ctrl);
void LED_Controller_SetLED(LED_Controller_t *ctrl, Input_LEDs led, Input_LED_Color_t color);

#endif // __LED_CONTROLLER_H__