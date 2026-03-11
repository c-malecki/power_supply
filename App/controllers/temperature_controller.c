#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//
#include "temperature_controller.h"
#include "ds18b20.h"
#include "main.h"
#include "ow.h"
#include "common.h"
#include "stm32f4xx_hal_gpio.h"

static ds18b20_t ds18;
static Temperature_Controller_t *t_ctrl = NULL;
volatile bool temp_ready = false;

void ds18_tim_cb(TIM_HandleTypeDef *htim)
{
    ow_callback(&ds18.ow);
}

void ds18_done_cb(ow_err_t err)
{
    if (err == OW_ERR_NONE && t_ctrl != NULL) {
        int16_t new_temp = ds18b20_read_c(&ds18);
        t_ctrl->cur_temp = new_temp;

        if (new_temp >= TEMP_CRITICAL) {
            HAL_GPIO_WritePin(RELAY_CHAN_PWR_GPIO_Port, RELAY_CHAN_PWR_Pin, GPIO_PIN_RESET);
        }

        temp_ready = true;
    }
}

_Error_Codes conv_ds18_error(ow_err_t ds18_error)
{
    _Error_Codes error = ERROR_NONE;

    switch (ds18_error) {

    case 0:
        break;
    case 1:
        error = ERROR_DS18_BUSY;
        break;
    case 2:
        error = ERROR_DS18_BUS;
        break;
    case 3:
        error = ERROR_DS18_RESET;
        break;
    case 4:
        error = ERROR_DS18_LEN;
        break;
    case 5:
        error = ERROR_DS18_ROM_ID;
        break;

    default:
        error = ERROR_UNKNOWN;
    }

    return error;
}

void Temperature_Controller_Ping_And_Init(Temperature_Controller_t *ctrl)
{
    ow_init_t ow_init_struct;
    ow_init_struct.tim_handle = &htim9;
    ow_init_struct.gpio = DS18B20_S_GPIO_Port;
    ow_init_struct.pin = DS18B20_S_Pin;
    ow_init_struct.tim_cb = ds18_tim_cb;
    ow_init_struct.done_cb = ds18_done_cb;

    ds18b20_init(&ds18, &ow_init_struct);

    _Error_Codes code = conv_ds18_error(ds18b20_update_rom_id(&ds18));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_UPDATE_ROM });
    }

    while (ds18b20_is_busy(&ds18))
        ;

    // Configure alarm thresholds and resolution
    ds18b20_config_t ds18_conf = { .alarm_high = 50,
                                   .alarm_low = -50,
                                   .cnv_bit = DS18B20_CNV_BIT_12 };

    code = conv_ds18_error(ds18b20_conf(&ds18, &ds18_conf));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_CONF });
    }

    while (ds18b20_is_busy(&ds18))
        ;

    ctrl->ds18 = ds18;

    t_ctrl = ctrl;
}

void Temperature_Controller_Read(Temperature_Controller_t *ctrl)
{
    _Error_Codes code = conv_ds18_error(ds18b20_cnv(&ctrl->ds18));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_CNV });
    }

    while (ds18b20_is_busy(&ctrl->ds18)) { };
    while (!ds18b20_is_cnv_done(&ctrl->ds18)) { };

    code = conv_ds18_error(ds18b20_req_read(&ctrl->ds18));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_CNV });
    }

    while (ds18b20_is_busy(&ctrl->ds18)) { };
}