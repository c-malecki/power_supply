#ifndef __DISPLAY_CONTROLLER_H__
#define __DISPLAY_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

#define DISPLAY_YELLOW_MAX_CHAR 14
#define DISPLAY_7x10_CLEAR_LINE "              "
#define DISPLAY_BLUE_MAX_CHAR 9
#define DISPLAY_11x18_CLEAR_LINE "         "

typedef enum {
    DISPLAY_BLUE_TOP = 0,
    DISPLAY_BLUE_BOT
} Display_Blue_Pos;

typedef struct
{
    I2C_HandleTypeDef *i2c_handle;
} Display_Controller_t;

_Error_Codes Display_Controller_PingPeripherals(I2C_HandleTypeDef *i2c_handle);
void Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle);
_Error_Codes Display_Controller_Write_Yellow(char *str);
_Error_Codes Display_Controller_Write_Blue(char *str, Display_Blue_Pos pos);
void Display_Controller_ShowVariableChannel(int32_t voltage_whole, uint32_t voltage_decimal,
                                            int32_t current_whole, uint32_t current_decimal);
void Display_Controller_State_Print(Display_Controller_t *ctrl);

#endif // __DISPLAY_CONTROLLER_H__