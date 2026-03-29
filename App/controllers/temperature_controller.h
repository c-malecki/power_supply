#ifndef __TEMPERATURE_CONTROLLER_H__
#define __TEMPERATURE_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

#define FAN_TEMP_OFF 27
#define FAN_TEMP_MIN 30
#define FAN_TEMP_MAX 50
#define FAN_PWM_MIN 800
#define FAN_PWM_MAX 3999

#define TEMP_CRITICAL 55
#define TEMP_INTERVAL 1000

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef enum {
    TEMP_CTRL_BUCK_SENSOR = 0,
    TEMP_CTRL_MOSFET_SENSOR
} Temperature_Controller_Sensors;

typedef enum {
    TEMP_CTRL_SENSOR_STATE_PREINIT = 0,
    TEMP_CTRL_SENSOR_STATE_READY,
    TEMP_CTRL_SENSOR_STATE_START_READ,
    TEMP_CTRL_SENSOR_STATE_WAIT_RESULT,
    TEMP_CTRL_SENSOR_STATE_GET_RESULT,
    TEMP_CTRL_SENSOR_STATE_WAIT_READ,
} Temperature_Controller_Sensor_States;

typedef struct
{
    int16_t last_temp;
    uint32_t last_read;
    uint32_t last_result;
    uint32_t i2c_addr;
    Temperature_Controller_Sensor_States state;
} Temperature_Controller_Sensor_t;

typedef enum {
    TEMP_CTRL_STATE_INIT,
    TEMP_CTRL_STATE_READY,
    TEMP_CTRL_STATE_START_READ,
    TEMP_CTRL_STATE_WAIT_RESULT,
    TEMP_CTRL_STATE_GET_RESULT,
    TEMP_CTRL_STATE_WAIT_READ,
} Temperature_Controller_States;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Temperature_Controller_Sensor_t sensors[2];
    uint32_t fan_last;
    uint32_t fan_cur;
    uint32_t fan_diff;
    uint32_t last_tick;
    float fan_rpm;
    uint8_t fan_first;
    bool fan_running;
    Temperature_Controller_States state;
    Error_Callback_t error_cb;
    void *error_ctx;
} Temperature_Controller_t;

void Temperature_Controller_SensorPing(Temperature_Controller_t *ctrl,
                                       Temperature_Controller_Sensors sensor);
void Temperature_Controller_SensorInit(Temperature_Controller_t *ctrl,
                                       Temperature_Controller_Sensors sensor);

void Temperature_Controller_Init(Temperature_Controller_t *ctrl, uint32_t addr,
                                 I2C_HandleTypeDef *i2c_handle);

void Temperature_Controller_SensorStartRead(Temperature_Controller_t *ctrl,
                                            Temperature_Controller_Sensors sensor);
void Temperature_Controller_SensorCheckResult(Temperature_Controller_t *ctrl,
                                              Temperature_Controller_Sensors sensor);
void Temperature_Controller_SensorGetResult(Temperature_Controller_t *ctrl,
                                            Temperature_Controller_Sensors sensor);
void Temperature_Controller_SensorCheckReady(Temperature_Controller_t *ctrl,
                                             Temperature_Controller_Sensors sensor);

void Temperature_Controller_UpdateFan(Temperature_Controller_t *ctrl);

void Temperature_ControllerRun(Temperature_Controller_t *ctrl);

#endif // __TEMPERATURE_CONTROLLER_H__