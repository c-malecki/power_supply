
#include "stm32f4xx_hal.h"
#include <stdio.h>
//
#include "tests.h"
#include "app.h"
#include "temperature_controller.h"

void Test_TemperatureSensor(App_t *app)
{
    printf("Test_TemperatureSensor\n**********\r\n\n");

    Temperature_Controller_Read(&app->temperature_controller);

    printf("temp: %d\r\n", app->temperature_controller.cur_temp);
}