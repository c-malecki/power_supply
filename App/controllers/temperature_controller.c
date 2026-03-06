#include <stdint.h>
#include <stdbool.h>
#include "temperature_controller.h"
#include "ds18b20.h"
#include "main.h"
#include "ow.h"

static ds18b20_t ds18;
static Temperature_Controller_t *t_ctrl = NULL;

void ds18_tim_cb(TIM_HandleTypeDef *htim)
{
    ow_callback(&ds18.ow);
}

void ds18_done_cb(ow_err_t err)
{
    if (err == OW_ERR_NONE) {
        t_ctrl->cur_temp = ds18b20_read_c(&ds18);
    }
}

uint8_t Temperature_Controller_Ping_And_Init(Temperature_Controller_t *ctrl)
{
    ow_init_t ow_init_struct;
    ow_init_struct.tim_handle = &htim9;
    ow_init_struct.gpio = DS18B20_S_GPIO_Port;
    ow_init_struct.pin = DS18B20_S_Pin;
    ow_init_struct.tim_cb = ds18_tim_cb;
    ow_init_struct.done_cb = ds18_done_cb;
    // ow_init_struct.rom_id_filter = DS18B20_ID;

    ds18b20_init(&ds18, &ow_init_struct);

    // Update ROM IDs for all devices
    ow_err_t err = ds18b20_update_rom_id(&ds18);
    if (err != 0) {
        return err;
    }

    while (ds18b20_is_busy(&ds18))
        ;

    // Configure alarm thresholds and resolution
    ds18b20_config_t ds18_conf = { .alarm_high = 50,
                                   .alarm_low = -50,
                                   .cnv_bit = DS18B20_CNV_BIT_12 };

    err = ds18b20_conf(&ds18, &ds18_conf);
    if (err != 0) {
        return err;
    }

    while (ds18b20_is_busy(&ds18))
        ;

    ctrl->ds18 = ds18;

    t_ctrl = ctrl;

    return 0;
}

uint8_t Temperature_Controller_Read(Temperature_Controller_t *ctrl)
{
    int16_t temp_c[2];

    ow_err_t err = ds18b20_cnv(&ctrl->ds18);

    while (ds18b20_is_busy(&ctrl->ds18))
        ;
    while (!ds18b20_is_cnv_done(&ctrl->ds18))
        ;

    if (err != 0) {
        return err;
    }

    err = ds18b20_req_read(&ctrl->ds18);
    while (ds18b20_is_busy(&ctrl->ds18))
        ;

    if (err != 0) {
        return err;
    }

    temp_c[0] = ds18b20_read_c(&ctrl->ds18);

    err = ds18b20_req_read(&ctrl->ds18);
    while (ds18b20_is_busy(&ctrl->ds18))
        ;

    if (err != 0) {
        return err;
    }

    temp_c[1] = ds18b20_read_c(&ctrl->ds18);

    // ctrl->cur_temp = (float)temp_c[0] + ((float)temp_c[1] / 100.0f);
}