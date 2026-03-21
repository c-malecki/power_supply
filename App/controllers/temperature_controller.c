#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
//
#include "temperature_controller.h"
#include "common.h"
#include "sht31.h"

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

    _Error_Codes code = SHT_Init(ctrl->i2c_handle);
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_TEMPERATURE,
                                    .peripheral = PERIPHERAL_SHT,
                                    .code = code,
                                    .function = FUNCTION_SHT_INIT });
    }
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
}