#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//
#include "temperature_controller.h"
#include "common.h"
#include "sht31.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"

void Temperature_Controller_PingSHT(Temperature_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    _Error_Codes code = ConvHALError(HAL_I2C_IsDeviceReady(i2c_handle, SHT_I2C_ADDRESS, 3, 100));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_SHT,
                                    .code = code,
                                    .function = FUNCTION_SHT_PING });
    }
}

void Temperature_Controller_Init(Temperature_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    ctrl->i2c_handle = i2c_handle;

    // initialize temperature sensor
    _Error_Codes code = SHT_Init(ctrl->i2c_handle);
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_SHT,
                                    .code = code,
                                    .function = FUNCTION_SHT_INIT });
    }

    // start input capture direct mode timer for tachometer
    code = ConvHALError(HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_FAN,
                                    .code = code,
                                    .function = FUNCTION_TIM_IC_START_IT });
    }

    // start pwm generation for fan control
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

void Temperature_Controller_UpdateFan(Temperature_Controller_t *ctrl)
{
    static bool fan_running = false;
    uint32_t pwm = 0;

    if (ctrl->cur_temp >= FAN_TEMP_MIN) {
        fan_running = true;
    } else if (ctrl->cur_temp <= FAN_TEMP_OFF) {
        fan_running = false;
    }

    if (!fan_running) {
        pwm = 0;
    } else if (ctrl->cur_temp >= FAN_TEMP_MAX) {
        pwm = FAN_PWM_MAX;
    } else {
        pwm = FAN_PWM_MIN
            + (ctrl->cur_temp - FAN_TEMP_MIN) * (FAN_PWM_MAX - FAN_PWM_MIN)
                / (FAN_TEMP_MAX - FAN_TEMP_MIN);
    }

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm);
}

void Temperature_Controller_Read(Temperature_Controller_t *ctrl)
{
    SHT_Result_t result = SHT_Read_Temperature(ctrl->i2c_handle);
    if (result.code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_SHT,
                                    .code = result.code,
                                    .function = FUNCTION_SHT_READ });
    }

    ctrl->cur_temp = result.temp_c;

    if (ctrl->cur_temp >= TEMP_CRITICAL) {
        // TODO: if cur temp too high, shut down
    }

    // check for a stall (no interrupt in 1 second) and reset
    if ((HAL_GetTick() - ctrl->last_tick) > 1000) {
        ctrl->fan_rpm = 0.0f;
        ctrl->fan_first = 1;
    }

    Temperature_Controller_UpdateFan(ctrl);
}