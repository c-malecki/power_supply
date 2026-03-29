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

void Temperature_Controller_SensorPing(Temperature_Controller_t *ctrl,
                                       Temperature_Controller_Sensors sensor)
{
    Temperature_Controller_Sensor_t s = ctrl->sensors[sensor];
    _Error_Codes code = ConvHALError(HAL_I2C_IsDeviceReady(ctrl->i2c_handle, s.i2c_addr, 3, 100));
    if (code != ERROR_NONE) {
        uint32_t prph =
            s.i2c_addr == SHT_I2C_BUCK_ADDRESS ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = prph,
                                    .code = code,
                                    .function = FUNCTION_SHT_PING });
    }
}

void Temperature_Controller_SensorInit(Temperature_Controller_t *ctrl,
                                       Temperature_Controller_Sensors sensor)
{
    Temperature_Controller_Sensor_t *s = &ctrl->sensors[sensor];

    _Error_Codes code = SHT_Init(s->i2c_addr, ctrl->i2c_handle);
    if (code != ERROR_NONE) {
        uint32_t prph =
            s->i2c_addr == SHT_I2C_BUCK_ADDRESS ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;

        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = prph,
                                    .code = code,
                                    .function = FUNCTION_SHT_INIT });
    }

    s->state = TEMP_CTRL_SENSOR_STATE_READY;
}

void Temperature_Controller_Init(Temperature_Controller_t *ctrl, uint32_t addr,
                                 I2C_HandleTypeDef *i2c_handle)
{
    ctrl->i2c_handle = i2c_handle;

    // initialize temperature sensor
    _Error_Codes code = SHT_Init(addr, ctrl->i2c_handle);
    if (code != ERROR_NONE) {
        uint32_t prph = addr == SHT_I2C_BUCK_ADDRESS ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;

        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = prph,
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

    ctrl->state = TEMP_CTRL_STATE_READY;
}

// TODO: test rpms after level shifter
void Temperature_Controller_UpdateFan(Temperature_Controller_t *ctrl)
{
    uint32_t pwm = 0;

    // TODO: Update to base off of two different temp readings

    // if (ctrl->cur_temp >= FAN_TEMP_MIN) {
    //     ctrl->fan_running = true;
    // } else if (ctrl->cur_temp <= FAN_TEMP_OFF) {
    //     ctrl->fan_running = false;
    // }

    // if (!ctrl->fan_running) {
    //     pwm = 0;
    // } else if (ctrl->cur_temp >= FAN_TEMP_MAX) {
    //     pwm = FAN_PWM_MAX;
    // } else {
    //     pwm = FAN_PWM_MIN
    //         + (ctrl->cur_temp - FAN_TEMP_MIN) * (FAN_PWM_MAX - FAN_PWM_MIN)
    //             / (FAN_TEMP_MAX - FAN_TEMP_MIN);
    // }

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pwm);
}

void Temperature_Controller_SensorStartRead(Temperature_Controller_t *ctrl,
                                            Temperature_Controller_Sensors sensor)
{
    Temperature_Controller_Sensor_t *s = &ctrl->sensors[sensor];
    s->state = TEMP_CTRL_SENSOR_STATE_START_READ;

    _Error_Codes code = SHT_StartRead(s->i2c_addr, ctrl->i2c_handle);
    if (code != ERROR_NONE) {
        uint32_t prph =
            s->i2c_addr == SHT_I2C_BUCK_ADDRESS ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;

        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = prph,
                                    .code = code,
                                    .function = FUNCTION_SHT_START_READ });
    }

    s->last_read = HAL_GetTick();
    s->state = TEMP_CTRL_SENSOR_STATE_WAIT_RESULT;
}

void Temperature_Controller_SensorCheckResult(Temperature_Controller_t *ctrl,
                                              Temperature_Controller_Sensors sensor)
{
    Temperature_Controller_Sensor_t *s = &ctrl->sensors[sensor];

    if (((HAL_GetTick() - s->last_read) >= 15)) {
        s->state = TEMP_CTRL_SENSOR_STATE_GET_RESULT;
    }
}

void Temperature_Controller_SensorGetResult(Temperature_Controller_t *ctrl,
                                            Temperature_Controller_Sensors sensor)
{
    Temperature_Controller_Sensor_t *s = &ctrl->sensors[sensor];

    SHT_Result_t result = SHT_GetResult(s->i2c_addr, ctrl->i2c_handle);
    if (result.code != ERROR_NONE) {
        uint32_t prph =
            s->i2c_addr == SHT_I2C_BUCK_ADDRESS ? PERIPHERAL_SHT_BUCK : PERIPHERAL_SHT_MOSFET;

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
    if ((HAL_GetTick() - ctrl->last_tick) > 1000) {
        ctrl->fan_rpm = 0.0f;
        ctrl->fan_first = 1;
    }

    Temperature_Controller_UpdateFan(ctrl);

    s->state = TEMP_CTRL_SENSOR_STATE_WAIT_READ;
}

void Temperature_Controller_SensorCheckReady(Temperature_Controller_t *ctrl,
                                             Temperature_Controller_Sensors sensor)
{
    Temperature_Controller_Sensor_t *s = &ctrl->sensors[sensor];

    if (((HAL_GetTick() - s->last_result) > 1000)) {
        s->last_read = 0;
        s->last_result = 0;
        s->state = TEMP_CTRL_SENSOR_STATE_READY;
    }
}

void Temperature_ControllerRun(Temperature_Controller_t *ctrl)
{
    switch (ctrl->state) {
    case TEMP_CTRL_STATE_INIT:
    case TEMP_CTRL_STATE_START_READ:
        break;
    case TEMP_CTRL_STATE_READY:
        Temperature_Controller_SensorStartRead(ctrl, SHT_I2C_BUCK_ADDRESS);
        Temperature_Controller_SensorStartRead(ctrl, SHT_I2C_MOSFET_ADDRESS);
        break;
    case TEMP_CTRL_STATE_WAIT_RESULT:
        Temperature_Controller_SensorCheckResult(ctrl, SHT_I2C_BUCK_ADDRESS);
        Temperature_Controller_SensorCheckResult(ctrl, SHT_I2C_MOSFET_ADDRESS);
        break;
    case TEMP_CTRL_STATE_GET_RESULT:
        Temperature_Controller_SensorGetResult(ctrl, SHT_I2C_BUCK_ADDRESS);
        Temperature_Controller_SensorGetResult(ctrl, SHT_I2C_MOSFET_ADDRESS);
        break;
    case TEMP_CTRL_STATE_WAIT_READ:
        Temperature_Controller_SensorCheckReady(ctrl, SHT_I2C_BUCK_ADDRESS);
        Temperature_Controller_SensorCheckReady(ctrl, SHT_I2C_MOSFET_ADDRESS);
        break;
    }
}