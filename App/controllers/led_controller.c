#include "stm32f411xe.h"
#include <stdint.h>
#include <stdbool.h>
#include "tim.h"
#include "led_controller.h"

const Input_LED_Color_t LED_COLOR_RED = { 255, 0, 0 };
const Input_LED_Color_t LED_COLOR_GREEN = { 0, 255, 0 };
const Input_LED_Color_t LED_COLOR_BLUE = { 0, 0, 255 };
const Input_LED_Color_t LED_COLOR_YELLOW = { 255, 255, 0 };
const Input_LED_Color_t LED_OFF = { 0, 0, 0 };

static const Input_LED_t led_status_default = { .r_htim = NULL, // htim2
                                                .r_timch = TIM_CHANNEL_2,
                                                .g_htim = NULL, // htim2
                                                .g_timch = TIM_CHANNEL_3,
                                                .b_htim = NULL, // htim2
                                                .b_timch = TIM_CHANNEL_4 };

static const Input_LED_t led_var_default = { .r_htim = NULL, // htim4
                                             .r_timch = TIM_CHANNEL_2,
                                             .g_htim = NULL, // htim4
                                             .g_timch = TIM_CHANNEL_1,
                                             .b_htim = NULL, // htim3
                                             .b_timch = TIM_CHANNEL_2 };

void LED_Controller_Init(LED_Controller_t *ctrl)
{
    ctrl->leds[LED_STATUS] = led_status_default;
    ctrl->leds[LED_STATUS].r_htim = &htim2;
    ctrl->leds[LED_STATUS].g_htim = &htim2;
    ctrl->leds[LED_STATUS].b_htim = &htim2;

    ctrl->leds[LED_VAR] = led_var_default;
    ctrl->leds[LED_VAR].r_htim = &htim4;
    ctrl->leds[LED_VAR].g_htim = &htim4;
    ctrl->leds[LED_VAR].b_htim = &htim3;

    for (int i = 0; i < LED_COUNT; i++) {
        HAL_TIM_PWM_Start(ctrl->leds[i].r_htim, ctrl->leds[i].r_timch);
        HAL_TIM_PWM_Start(ctrl->leds[i].g_htim, ctrl->leds[i].g_timch);
        HAL_TIM_PWM_Start(ctrl->leds[i].b_htim, ctrl->leds[i].b_timch);
    }

    LED_Controller_SetLED(ctrl, LED_STATUS, LED_COLOR_YELLOW);
}

void LED_Controller_SetLED(LED_Controller_t *ctrl, Input_LEDs led, Input_LED_Color_t color)
{
    __HAL_TIM_SET_COMPARE(ctrl->leds[led].r_htim, ctrl->leds[led].r_timch, color.red);
    __HAL_TIM_SET_COMPARE(ctrl->leds[led].g_htim, ctrl->leds[led].g_timch, color.green);
    __HAL_TIM_SET_COMPARE(ctrl->leds[led].b_htim, ctrl->leds[led].b_timch, color.blue);
}