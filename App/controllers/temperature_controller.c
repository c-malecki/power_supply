#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//
#include "temperature_controller.h"
#include "common.h"
#include "ds18b20.h"
#include "ow.h"

static ds18b20_t ds18;
static Temperature_Controller_t *t_ctrl = NULL;

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

void Temperature_Controller_TimCB(TIM_HandleTypeDef *htim)
{
    ow_callback(&ds18.ow);
}

void Temperature_Controller_DoneCB(ow_err_t error)
{
    if (t_ctrl == NULL) {
        return;
    };

    _Error_Codes code = conv_ds18_error(error);
    if (code != ERROR_NONE) {
        t_ctrl->error_cb(t_ctrl->error_ctx,
                         (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                      .peripheral = PERIPHERAL_DS18,
                                      .code = code,
                                      .function = FUNCTION_DS18_REQ_READ });
    }

    if (t_ctrl->cur_state == TEMP_STATE_REQ_READ && t_ctrl->prev_state == TEMP_STATE_CNV) {
        Temperature_Controller_EndRead(t_ctrl);
    }
}

void Temperature_Controller_PingAndInit(Temperature_Controller_t *ctrl)
{
    ctrl->prev_state = TEMP_STATE_INIT;
    ctrl->cur_state = TEMP_STATE_INIT;

    ow_init_t ow_init_struct;
    ow_init_struct.tim_handle = &htim1;
    ow_init_struct.gpio = GPIO_DS18B20_GPIO_Port;
    ow_init_struct.pin = GPIO_DS18B20_Pin;
    ow_init_struct.tim_cb = Temperature_Controller_TimCB;
    ow_init_struct.done_cb = Temperature_Controller_DoneCB;

    ctrl->ds18 = &ds18;
    t_ctrl = ctrl;
    ds18b20_init(&ds18, &ow_init_struct);

    _Error_Codes code = conv_ds18_error(ds18b20_update_rom_id(ctrl->ds18));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_UPDATE_ROM });
    }

    while (ds18b20_is_busy(ctrl->ds18))
        ;

    ds18b20_config_t ds18_conf = { .alarm_high = 50,
                                   .alarm_low = -50,
                                   .cnv_bit = DS18B20_CNV_BIT_12 };

    code = conv_ds18_error(ds18b20_conf(ctrl->ds18, &ds18_conf));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_CONF });
    }

    while (ds18b20_is_busy(ctrl->ds18))
        ;

    ctrl->cur_state = TEMP_STATE_READY;
}

void Temperature_Controller_StartCnv(Temperature_Controller_t *ctrl)
{
    if (ds18b20_is_busy(ctrl->ds18)) {
        return;
    }

    _Error_Codes code = conv_ds18_error(ds18b20_cnv(ctrl->ds18));
    // ow becomes state xfer
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_CNV });
    }

    ctrl->prev_state = TEMP_STATE_READY;
    ctrl->cur_state = TEMP_STATE_CNV;
}

void Temperature_Controller_ReqRead(Temperature_Controller_t *ctrl)
{
    // ow is state xfer
    if (!ds18b20_is_cnv_done(ctrl->ds18)) {
        return;
    }

    _Error_Codes code = conv_ds18_error(ds18b20_req_read(ctrl->ds18));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_REQ_READ });
    }

    ctrl->prev_state = TEMP_STATE_CNV;
    ctrl->cur_state = TEMP_STATE_REQ_READ;
}

void Temperature_Controller_EndRead(Temperature_Controller_t *ctrl)
{
    _Error_Codes code = conv_ds18_error(ds18b20_req_read(ctrl->ds18));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_DS18,
                                    .code = code,
                                    .function = FUNCTION_DS18_REQ_READ });
    }

    int16_t new_temp = ds18b20_read_c(ctrl->ds18);
    ctrl->cur_temp = new_temp;
    ctrl->prev_state = TEMP_STATE_REQ_READ;
    ctrl->cur_state = TEMP_STATE_READ_C;

    // TODO: add fan speed control
    if (new_temp >= TEMP_CRITICAL) {
        // TODO: figure out all safety mechanisms
        HAL_GPIO_WritePin(GPIO_RELAY_S_GPIO_Port, GPIO_RELAY_S_Pin, GPIO_PIN_RESET);
    }

    ctrl->prev_state = TEMP_STATE_READ_C;
    ctrl->cur_state = TEMP_STATE_READY;
}

bool Temperature_Controller_CheckReady(Temperature_Controller_t *ctrl)
{
    return ds18b20_is_busy(ctrl->ds18);
}