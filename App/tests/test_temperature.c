
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
//
#include "tests.h"
#include "app.h"
#include "temperature_controller.h"

void Test_Temperature(App_t *app)
{
    Temp_Ctrl_t *ctrl = &app->temperature_controller;

    printf("TEST TEMP: begin\n**********\r\n\n");

    for (uint8_t i = 0; i < TEMP_SENSOR_COUNT; i++) {
        bool done = false;
        Temp_Sensor idx = (Temp_Sensor)i;
        Temp_Sensor_t *sensor = &app->temperature_controller.sensors[idx];
        char *str = "";

        if (idx == TEMP_SENSOR_BUCK) {
            str = "BUCK SENSOR";
        } else {
            str = "MOSFET SENSOR";
        }

        while (!done) {
            switch (sensor->state) {
            case TEMP_SENSOR_STATE_INIT:
            case TEMP_SENSOR_STATE_START_READ:
            case TEMP_SENSOR_STATE_WAIT_READ:
                break;

            case TEMP_SENSOR_STATE_READY:
                Temp_Sensor_StartRead(&app->temperature_controller, idx);
                break;

            case TEMP_SENSOR_STATE_WAIT_RESULT:
                Temp_Sensor_CheckResult(&app->temperature_controller, idx);
                break;

            case TEMP_SENSOR_STATE_GET_RESULT:
                Temp_Sensor_GetResult(&app->temperature_controller, idx);
                done = true;
                break;
            }
        }

        printf("%s:\ntemp: %d\r\n\n", str, sensor->last_temp);
    }

    static const int16_t temps[] = { 25, 30, 35, 40, 45, 50 };
    static const uint8_t temp_count = 6;
    uint8_t temp_idx = 0;
    uint32_t start = HAL_GetTick();

    ctrl->sensors[0].last_temp = temps[temp_idx];
    ctrl->sensors[1].last_temp = temps[temp_idx];

    Temp_Fan_Update(ctrl);

    while (1) {
        if (HAL_GetTick() - start >= 5000) {
            start = HAL_GetTick();
            temp_idx = (temp_idx + 1) % temp_count;
            ctrl->sensors[0].last_temp = temps[temp_idx];
            ctrl->sensors[1].last_temp = temps[temp_idx];
            Temp_Fan_Update(ctrl);

            printf("Fan:\non: %u\nvalue_last: %u\nvalue_cur: "
                   "%u\nvalue_diff: "
                   "%u\nlast_tick: %u\nrpm: %.04f\r\n\n",
                   ctrl->fan.on, ctrl->fan.value_last, ctrl->fan.value_cur, ctrl->fan.value_diff,
                   ctrl->fan.last_tick, ctrl->fan.rpm);
        }
    }

    printf("TEST TEMP: end\n**********\r\n\n");
}