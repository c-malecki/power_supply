
#include "stm32f4xx_hal.h"
#include <stdio.h>
//
#include "tests.h"
#include "app.h"
#include "temperature_controller.h"

void print_ctrl(Temperature_Controller_t *ctrl)
{
    printf("Temperature_Controller:\ncur_temp: %d\nfan_running: %u\nfan_last: %u\nfan_cur: "
           "%u\nfan_diff: "
           "%u\nlast_tick: %u\nfan_rpm: %.04f\r\n\n",
           ctrl->cur_temp, ctrl->fan_running, ctrl->fan_last, ctrl->fan_cur, ctrl->fan_diff,
           ctrl->last_tick, ctrl->fan_rpm);
}

void Test_TemperatureController(App_t *app)
{
    Temperature_Controller_t *ctrl = &app->temperature_controller;

    printf("Test_TemperatureController\n**********\r\n\n");

    // while (1) {
    //     switch (ctrl->state) {
    //     case TEMP_CTRL_STATE_INIT:
    //     case TEMP_CTRL_STATE_START_READ:
    //         break;
    //     case TEMP_CTRL_STATE_READY:
    //         Temperature_Controller_StartRead(ctrl);
    //         break;
    //     case TEMP_CTRL_STATE_WAIT_RESULT:
    //         Temperature_Controller_CheckResult(ctrl);
    //         break;
    //     case TEMP_CTRL_STATE_GET_RESULT:
    //         Temperature_Controller_GetResult(ctrl);
    //         print_ctrl(ctrl);
    //         break;
    //     case TEMP_CTRL_STATE_WAIT_READ:
    //         Temperature_Controller_CheckReady(ctrl);
    //         break;
    //     }
    // }

    printf("Test Fan\n**********\r\n\n");

    static const int16_t temps[] = { 25, 30, 35, 40, 45, 50 };
    static const uint8_t temp_count = 6;
    uint8_t temp_idx = 0;
    uint32_t start = HAL_GetTick();

    ctrl->cur_temp = temps[temp_idx];
    Temperature_Controller_UpdateFan(ctrl);
    print_ctrl(ctrl);

    while (1) {
        if (HAL_GetTick() - start >= 5000) {
            start = HAL_GetTick();
            temp_idx = (temp_idx + 1) % temp_count;
            ctrl->cur_temp = temps[temp_idx];
            Temperature_Controller_UpdateFan(ctrl);
            print_ctrl(ctrl);
        }
    }
}