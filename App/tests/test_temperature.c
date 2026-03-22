
#include "stm32f4xx_hal.h"
#include <stdio.h>
//
#include "tests.h"
#include "app.h"
#include "temperature_controller.h"

void print_ctrl(Temperature_Controller_t *ctrl)
{
    printf("Temperature_Controller:\ncur_temp: %d\nfan_last: %u\nfan_cur: %u\nfan_diff: "
           "%u\nlast_tick: %u\nfan_rpm: %.04f\r\n",
           ctrl->cur_temp, ctrl->fan_last, ctrl->fan_cur, ctrl->fan_diff, ctrl->last_tick,
           ctrl->fan_rpm);
}

void Test_TemperatureController(App_t *app)
{
    printf("Test_TemperatureController\n**********\r\n");

    Temperature_Controller_Read(&app->temperature_controller);

    printf("current temp: %d\r\n", app->temperature_controller.cur_temp);

    printf("Test Fan\n**********\r\n");

    printf("fan temp min: %d\r\n\n", 30);
    app->temperature_controller.cur_temp = 30;
    Temperature_Controller_UpdateFan(&app->temperature_controller);

    print_ctrl(&app->temperature_controller);
}