
#include "stm32f4xx_hal.h"
#include <stdio.h>
//
#include "tests.h"
#include "app.h"
#include "led_controller.h"

void Test_LEDs(App_t *app)
{
    printf("Test_LEDs\n**********\r\n\n");

    printf("STATUS LED\n");

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_RED);
    printf("red\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_GREEN);
    printf("green\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_BLUE);
    printf("blue\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_YELLOW);
    printf("yellow\r\n\n");
    HAL_Delay(4000);

    printf("VAR LED\r\n");

    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_RED);
    printf("red\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_GREEN);
    printf("green\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_BLUE);
    printf("blue\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_YELLOW);
    printf("yellow\r\n\n");
    HAL_Delay(4000);

    printf("5V LED\r\n");

    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_RED);
    printf("red\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_GREEN);
    printf("green\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_BLUE);
    printf("blue\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_YELLOW);
    printf("yellow\r\n\n");
    HAL_Delay(4000);

    // LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_RED);
    // // LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_COLOR_YELLOW);
    // LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_GREEN);
    // LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_BLUE);
    // printf("Status LED: red\n5V LED: green\nVar LED: blue\r\n\n");
    // HAL_Delay(5000);

    // LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_YELLOW);
    // // LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_COLOR_BLUE);
    // LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_RED);
    // LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_GREEN);
    // printf("Status LED: yellow\n5V LED: red\nVar LED: green\r\n\n");
    // HAL_Delay(5000);

    // LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_BLUE);
    // // LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_COLOR_GREEN);
    // LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_YELLOW);
    // LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_RED);
    // printf("Status LED: blue\n5V LED: yellow\nVar LED: red\r\n\n");
    // HAL_Delay(5000);

    // LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_GREEN);
    // // LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_COLOR_RED);
    // LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_COLOR_BLUE);
    // LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_COLOR_YELLOW);
    // printf("Status LED: green\n5V LED: blue\nVar LED: yellow\r\n\n");
    // HAL_Delay(5000);

    // LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_OFF);
    // // LED_Controller_SetLED(&app->led_ctrl, LED_3V3, LED_OFF);
    // LED_Controller_SetLED(&app->led_ctrl, LED_5V, LED_OFF);
    // LED_Controller_SetLED(&app->led_ctrl, LED_VAR, LED_OFF);
    // printf("Status LED: off\n5V LED: off\nVar LED: off\r\n\n");
    // HAL_Delay(5000);

    LED_Controller_SetLED(&app->led_ctrl, LED_STATUS, LED_COLOR_GREEN);
}