#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

#define DISPLAY_TOP_ROW 3
#define DISPLAY_TOP_COL_1 2
#define DISPLAY_TOP_COL_2 34
#define DISPLAY_TOP_COL_3 66
#define DISPLAY_TOP_COL_4 98

#define DISPLAY_BOT_ROW_1 20
#define DISPLAY_BOT_ROW_2 42
#define DISPLAY_BOT_COL_1 2
#define DISPLAY_BOT_COL_2 82

typedef void (*Error_Callback_t)(void *ctx, _Error_t error);

typedef enum {
    DISPLAY_BLUE_TOP = 0,
    DISPLAY_BLUE_BOT
} Display_Blue_Pos;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
    Error_Callback_t error_cb;
    void *error_ctx;
} Display_Controller_t;

void Display_Controller_PingGME(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
void Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);

void Display_Controller_Write_Yellow(Display_Controller_t *ctrl, char *str);
void Display_Controller_Write_Blue(Display_Controller_t *ctrl, char *str, Display_Blue_Pos pos);

void Display_Controller_ShowVariableChannel(Display_Controller_t *ctrl, int32_t voltage_whole,
                                            uint32_t voltage_decimal, int32_t current_whole,
                                            uint32_t current_decimal);

void Display_Controller_State_Print(Display_Controller_t *ctrl);

#endif // __DISPLAY_CONTROLLER_H__