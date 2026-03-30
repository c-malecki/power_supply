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

static const Temp_Sensor_t buck_sensor_default = { .last_temp = 0,
                                                   .last_read = 0,
                                                   .last_result = 0,
                                                   .i2c_addr = SHT_I2C_ADDR_BUCK,
                                                   .state = TEMP_SENSOR_STATE_INIT };

static const Temp_Sensor_t mosfet_sensor_default = { .last_temp = 0,
                                                     .last_read = 0,
                                                     .last_result = 0,
                                                     .i2c_addr = SHT_I2C_ADDR_MOSFET,
                                                     .state = TEMP_SENSOR_STATE_INIT };

static const Temp_Fan_t fan_default = { .value_last = 0,
                                        .value_cur = 0,
                                        .value_diff = 0,
                                        .last_tick = 0,
                                        .rpm = 0,
                                        .first_cb = true,
                                        .on = false };

void Temp_Ctrl_Init(Temp_Ctrl_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    ctrl->i2c_handle = i2c_handle;
    ctrl->sensors[0] = buck_sensor_default;
    ctrl->sensors[1] = mosfet_sensor_default;
    ctrl->fan = fan_default;
}

void Temp_Ctrl_Ping(Temp_Ctrl_t *ctrl)
{
    _Error_Codes code;

    for (uint8_t i = 0; i < TEMP_SENSOR_COUNT; i++) {
        code = ConvHALError(
            HAL_I2C_IsDeviceReady(ctrl->i2c_handle, ctrl->sensors[i].i2c_addr, 3, 100));
        if (code != ERROR_NONE) {
            ctrl->error_cb(ctrl->error_ctx,
                           (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                        .peripheral = SHT_I2C_ADDR_BUCK,
                                        .code = code,
                                        .function = FUNCTION_SHT_PING });
        }
    }
}

void Temp_Ctrl_Run(Temp_Ctrl_t *ctrl)
{
    // init all sensors
    for (uint8_t i = 0; i < TEMP_SENSOR_COUNT; i++) {
        Temp_Sensor_t *s = &ctrl->sensors[i];

        _Error_Codes code = SHT_Init(s->i2c_addr, ctrl->i2c_handle);
        if (code != ERROR_NONE) {
            uint32_t prph =
                s->i2c_addr == SHT_I2C_ADDR_BUCK ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;

            ctrl->error_cb(ctrl->error_ctx,
                           (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                        .peripheral = prph,
                                        .code = code,
                                        .function = FUNCTION_SHT_INIT });
        }

        s->state = TEMP_SENSOR_STATE_READY;
    }

    // start input capture direct mode timer for tachometer
    _Error_Codes code = ConvHALError(HAL_TIM_IC_Start_IT(&htim5, TIM_CHANNEL_2));
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

// TODO: test rpms after level shifter
void Temp_Fan_Update(Temp_Ctrl_t *ctrl)
{
    uint32_t pwm = 0;
    Temp_Sensor_t *sb = &ctrl->sensors[TEMP_SENSOR_BUCK];
    Temp_Sensor_t *sm = &ctrl->sensors[TEMP_SENSOR_MOSFET];

    if (sb->last_temp >= FAN_TEMP_MIN || sm->last_temp >= FAN_TEMP_MIN) {
        ctrl->fan.on = true;
    } else if (sb->last_temp <= FAN_TEMP_OFF || sm->last_temp <= FAN_TEMP_OFF) {
        ctrl->fan.on = false;
    }

    if (!ctrl->fan.on) {
        pwm = 0;
    } else if (sb->last_temp >= FAN_TEMP_MAX || sm->last_temp >= FAN_TEMP_MAX) {
        pwm = FAN_PWM_MAX;
    } else {
        int16_t highest = sb->last_temp;
        if (highest < sm->last_temp) {
            highest = sm->last_temp;
        }

        pwm = FAN_PWM_MIN
            + (highest - FAN_TEMP_MIN) * (FAN_PWM_MAX - FAN_PWM_MIN)
                / (FAN_TEMP_MAX - FAN_TEMP_MIN);
    }

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm);
}

void Temp_Sensor_StartRead(Temp_Ctrl_t *ctrl, Temp_Sensor sensor)
{
    Temp_Sensor_t *s = &ctrl->sensors[sensor];
    s->state = TEMP_SENSOR_STATE_START_READ;

    _Error_Codes code = SHT_StartRead(s->i2c_addr, ctrl->i2c_handle);
    if (code != ERROR_NONE) {
        uint32_t prph =
            s->i2c_addr == SHT_I2C_ADDR_BUCK ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;

        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = prph,
                                    .code = code,
                                    .function = FUNCTION_SHT_START_READ });
    }

    s->last_read = HAL_GetTick();
    s->state = TEMP_SENSOR_STATE_WAIT_RESULT;
}

void Temp_Sensor_CheckResult(Temp_Ctrl_t *ctrl, Temp_Sensor sensor)
{
    Temp_Sensor_t *s = &ctrl->sensors[sensor];

    if (((HAL_GetTick() - s->last_read) >= 15)) {
        s->state = TEMP_SENSOR_STATE_GET_RESULT;
    }
}

void Temp_Sensor_GetResult(Temp_Ctrl_t *ctrl, Temp_Sensor sensor)
{
    Temp_Sensor_t *s = &ctrl->sensors[sensor];

    SHT_Result_t result = SHT_GetResult(s->i2c_addr, ctrl->i2c_handle);
    if (result.code != ERROR_NONE) {
        uint32_t prph =
            s->i2c_addr == SHT_I2C_ADDR_BUCK ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;

        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = prph,
                                    .code = result.code,
                                    .function = FUNCTION_SHT_GET_RESULT });
    }

    s->last_temp = result.temp_c;
    s->last_result = HAL_GetTick();

    if (s->last_temp >= TEMP_CRITICAL) {
        // TODO: if cur temp too high, shut down
    }

    // check for a stall (no interrupt in 1 second) and reset
    if ((HAL_GetTick() - ctrl->fan.last_tick) > 1000) {
        ctrl->fan.rpm = 0.0f;
        ctrl->fan.first_cb = 1;
    }

    Temp_Fan_Update(ctrl);

    s->state = TEMP_SENSOR_STATE_WAIT_READ;
}

void Temp_Sensor_CheckReady(Temp_Ctrl_t *ctrl, Temp_Sensor sensor)
{
    Temp_Sensor_t *s = &ctrl->sensors[sensor];

    if (((HAL_GetTick() - s->last_result) > 1000)) {
        s->last_read = 0;
        s->last_result = 0;
        s->state = TEMP_SENSOR_STATE_READY;
    }
}