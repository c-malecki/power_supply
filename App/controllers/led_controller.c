#include "stm32f411xe.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "tim.h"
#include "led_controller.h"

static uint32_t buf[LED_BUF_SIZE];

const LED_Color_t LED_COLOR_RED = { 255, 0, 0 };
const LED_Color_t LED_COLOR_GREEN = { 0, 255, 0 };
const LED_Color_t LED_COLOR_BLUE = { 0, 0, 255 };
const LED_Color_t LED_COLOR_YELLOW = { 255, 255, 0 };
const LED_Color_t LED_OFF = { 0, 0, 0 };

void LED_Controller_Init(LED_Controller_t *ctrl, TIM_HandleTypeDef *htim)
{
    ctrl->htim = htim;
    memset(buf, 0, sizeof(buf));
}

void LED_Controller_SetLED(LED_Controller_t *ctrl, LED_IDX_t idx, LED_Color_t color)
{
    uint32_t base = idx * LED_BITS_PER;
    uint8_t bytes[3] = { color.green, color.red, color.blue };

    for (uint8_t i = 0; i < 3; i++) {
        for (uint8_t bit = 0; bit < 8; bit++) {
            buf[base + i * 8 + bit] = (bytes[i] & (0x80 >> bit)) ? LED_BIT_1 : LED_BIT_0;
        }
    }

    ctrl->leds[idx] = color;
}

void LED_Controller_ShowLED(LED_Controller_t *ctrl)
{
    HAL_TIM_PWM_Start_DMA(ctrl->htim, TIM_CHANNEL_2, buf, LED_BUF_SIZE);
}