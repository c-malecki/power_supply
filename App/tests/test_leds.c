
#include "stm32f4xx_hal.h"
#include <stdio.h>
//
#include "tests.h"
#include "app.h"
#include "led_controller.h"

void Test_LEDs(App_t *app)
{
    printf("Test_LEDs\n**********\r\n\n");

    printf("MAIN LED\n");

    LED_Controller_SetLED(&app->led_controller, LED_MAIN, LED_COLOR_RED);
    printf("red\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_MAIN, LED_COLOR_GREEN);
    printf("green\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_MAIN, LED_COLOR_BLUE);
    printf("blue\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_MAIN, LED_COLOR_YELLOW);
    printf("yellow\r\n\n");
    HAL_Delay(4000);

    //

    printf("3V LED\r\n");

    LED_Controller_SetLED(&app->led_controller, LED_3V3, LED_COLOR_RED);
    printf("red\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_3V3, LED_COLOR_GREEN);
    printf("green\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_3V3, LED_COLOR_BLUE);
    printf("blue\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_3V3, LED_COLOR_YELLOW);
    printf("yellow\r\n\n");
    HAL_Delay(4000);

    //

    printf("5V LED\r\n");

    LED_Controller_SetLED(&app->led_controller, LED_5V, LED_COLOR_RED);
    printf("red\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_5V, LED_COLOR_GREEN);
    printf("green\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_5V, LED_COLOR_BLUE);
    printf("blue\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_5V, LED_COLOR_YELLOW);
    printf("yellow\r\n\n");
    HAL_Delay(4000);

    //

    printf("VAR LED\r\n");

    LED_Controller_SetLED(&app->led_controller, LED_VAR, LED_COLOR_RED);
    printf("red\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_VAR, LED_COLOR_GREEN);
    printf("green\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_VAR, LED_COLOR_BLUE);
    printf("blue\r\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_VAR, LED_COLOR_YELLOW);
    printf("yellow\r\n\n");
    HAL_Delay(4000);

    LED_Controller_SetLED(&app->led_controller, LED_MAIN, LED_COLOR_GREEN);
}