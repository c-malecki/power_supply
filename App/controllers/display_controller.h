#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include "common.h"

#define DSP_TOP_ROW 3
#define DSP_TOP_COL_1 2
#define DSP_TOP_COL_2 34
#define DSP_TOP_COL_3 66
#define DSP_TOP_COL_4 98

#define DSP_BOT_ROW_1 20
#define DSP_BOT_ROW_2 42
#define DSP_BOT_COL_1 2
#define DSP_BOT_COL_2 82

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef enum {
    DSP_MENU = 0,
    DSP_VAR,
    DSP_COUNT
} Dsp_Oled;

typedef enum {
    DSP_BLUE_TOP = 0,
    DSP_BLUE_BOT
} Dsp_Blue_Pos;

typedef struct
{
    bool update_pending;
    uint32_t last_update;
    uint32_t i2c_addr;
} Dsp_Oled_t;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Dsp_Oled_t oleds[2];
    Error_Callback_t error_cb;
    void *error_ctx;
} Dsp_Ctrl_t;

void Dsp_Ctrl_Init(Dsp_Ctrl_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Dsp_Ctrl_Ping(Dsp_Ctrl_t *ctrl);
void Dsp_Ctrl_Run(Dsp_Ctrl_t *ctrl);

void Dsp_Ctrl_WriteYellow(Dsp_Ctrl_t *ctrl, char *str);
void Dsp_Ctrl_WriteBlue(Dsp_Ctrl_t *ctrl, char *str, Dsp_Blue_Pos pos);

void Dsp_Ctrl_ShowVariableChannel(Dsp_Ctrl_t *ctrl, int32_t voltage_whole, uint32_t voltage_decimal,
                                  int32_t current_whole, uint32_t current_decimal);

#endif // __DISPLAY_CONTROLLER_H__