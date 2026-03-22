#ifndef __LED_CONTROLLER_H__
#define __LED_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"
#include "stm32f4xx_hal_tim.h"

#define LED_COUNT 4
#define LED_BITS_PER 24
#define LED_RESET_CYCLES 40
#define LED_BIT_0 40
#define LED_BIT_1 80
#define LED_BUF_SIZE (LED_COUNT * LED_BITS_PER + LED_RESET_CYCLES)

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef enum {
    LED_MAIN,
    LED_3V3,
    LED_5V,
    LED_VAR,
} LED_IDX_t;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} LED_Color_t;

extern const LED_Color_t LED_COLOR_RED;
extern const LED_Color_t LED_COLOR_GREEN;
extern const LED_Color_t LED_COLOR_BLUE;
extern const LED_Color_t LED_COLOR_YELLOW;
extern const LED_Color_t LED_OFF;

typedef struct
{
    Error_Callback_t error_cb;
    TIM_HandleTypeDef *htim;
    LED_Color_t leds[4];
    void *error_ctx;
} LED_Controller_t;

void LED_Controller_Init(LED_Controller_t *ctrl, TIM_HandleTypeDef *htim);
void LED_Controller_SetLED(LED_Controller_t *ctrl, LED_IDX_t idx, LED_Color_t color);
void LED_Controller_ShowLED(LED_Controller_t *ctrl);

#endif // __LED_CONTROLLER_H__