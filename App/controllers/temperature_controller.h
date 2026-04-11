#ifndef __Temp_Ctrl_H__
#define __Temp_Ctrl_H__

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
    TEMP_SENSOR_BUCK = 0,
    TEMP_SENSOR_MOSFET,
    TEMP_SENSOR_COUNT,
} Temp_Sensor;

typedef enum {
    TEMP_SENSOR_STATE_INIT = 0,
    TEMP_SENSOR_STATE_READY,
    TEMP_SENSOR_STATE_START_READ,
    TEMP_SENSOR_STATE_WAIT_RESULT,
    TEMP_SENSOR_STATE_GET_RESULT,
    TEMP_SENSOR_STATE_WAIT_READ,
} Temp_Sensor_State;

typedef struct
{
    int16_t last_temp;
    uint32_t last_read;
    uint32_t last_result;
    uint32_t i2c_addr;
    Temp_Sensor_State state;
} Temp_Sensor_t;

typedef struct
{
    uint32_t value_last;
    uint32_t value_cur;
    uint32_t value_diff;
    uint32_t last_tick;
    float rpm;
    bool first_cb;
    bool on;
} Temp_Fan_t;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Temp_Sensor_t sensors[2];
    Temp_Fan_t fan;
    Error_Callback_t error_cb;
    void *error_ctx;
} Temp_Ctrl_t;

void Temp_Ctrl_Init(Temp_Ctrl_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Temp_Ctrl_Ping(Temp_Ctrl_t *ctrl);
void Temp_Ctrl_Run(Temp_Ctrl_t *ctrl);

void Temp_Sensor_StartRead(Temp_Ctrl_t *ctrl, Temp_Sensor sensor);
void Temp_Sensor_CheckResult(Temp_Ctrl_t *ctrl, Temp_Sensor sensor);
void Temp_Sensor_GetResult(Temp_Ctrl_t *ctrl, Temp_Sensor sensor);
void Temp_Sensor_CheckReady(Temp_Ctrl_t *ctrl, Temp_Sensor sensor);

void Temp_Fan_Update(Temp_Ctrl_t *ctrl);

#endif // __Temp_Ctrl_H__