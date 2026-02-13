#include "app.h"
#include "chan.h"
#include "MCP4725.h"
#include "INA219.h"

PWR_Chan_t chan_3v3;
PWR_Chan_t chan_5v;
PWR_Chan_t chan_vdc;

APP_Status_t App_Init(APP_t *app, I2C_HandleTypeDef *i2c_handle)
{
    APP_Status_t status;
    status.ctrl = APP_CTRL;
    status.err = 0;

    app->i2c_handle = i2c_handle;

    PWR_Chan_Init(&chan_3v3, 3.3, NULL, 0);
    PWR_Chan_Init(&chan_5v, 5.0, NULL, 0);
    PWR_Chan_Init(&chan_vdc, 0.0, NULL, 0);

    app->pwr_chans[CHAN_3V3] = &chan_3v3;
    app->pwr_chans[CHAN_5V] = &chan_5v;
    app->pwr_chans[CHAN_VDC] = &chan_vdc;

    HAL_StatusTypeDef err = INA_Init(i2c_handle);
    if (err != 0) {
        status.ctrl = PWR_CTRL;
        status.err = err;
        return status;
    }

    return status;
}