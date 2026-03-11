#include "stm32f411xe.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "display_controller.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "ssd1306_conf.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "common.h"

void Display_Controller_PingGME(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    _Error_Codes code = ConvHALError(HAL_I2C_IsDeviceReady(i2c_handle, SSD1306_I2C_ADDR, 3, 100));
    if (code != ERROR_NONE) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_DISPLAY,
                                    .peripheral = PERIPHERAL_GME,
                                    .code = code,
                                    .function = FUNCTION_GME_PING });
    }
}

void Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    char *text = "Initializing...";
    ssd1306_Init();
    ssd1306_SetCursor(12, 27);
    ssd1306_WriteString(text, Font_7x10, White);
    ssd1306_UpdateScreen();
}

void Display_Controller_Write_Yellow(Display_Controller_t *ctrl, char *str)
{
    size_t len = strlen(str);
    if (len == 0 || len > DISPLAY_YELLOW_MAX_CHAR) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_DISPLAY,
                                    .peripheral = PERIPHERAL_GME,
                                    .code = ERROR_GME_INVALID_STR_LEN,
                                    .function = FUNCTION_GME_WRITE_YELLOW });
    }

    uint8_t x = 2;
    uint8_t y = 3;

    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(DISPLAY_7x10_CLEAR_LINE, Font_7x10, White);

    for (int i = 0; i < len; i++) {
        ssd1306_SetCursor(x, y);
        char v = ssd1306_WriteChar(str[i], Font_7x10, White);
        if (v == 0) {
            ctrl->error_cb(ctrl->error_ctx,
                           (_Error_t) { .controller = CONTROLLER_DISPLAY,
                                        .peripheral = PERIPHERAL_GME,
                                        .code = ERROR_GME_INVALID_CHAR,
                                        .function = FUNCTION_GME_WRITE_CHAR });
        }
        x += Font_7x10.width + 2;
    }

    ssd1306_UpdateScreen();
}

void Display_Controller_Write_Blue(Display_Controller_t *ctrl, char *str, Display_Blue_Pos pos)
{
    size_t len = strlen(str);
    if (len == 0 || len > DISPLAY_BLUE_MAX_CHAR) {
        ctrl->error_cb(ctrl->error_ctx,
                       (_Error_t) { .controller = CONTROLLER_DISPLAY,
                                    .peripheral = PERIPHERAL_GME,
                                    .code = ERROR_GME_INVALID_STR_LEN,
                                    .function = FUNCTION_GME_WRITE_BLUE });
    }

    uint8_t x = 3;
    uint8_t y = pos == DISPLAY_BLUE_TOP ? 20 : 42;

    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(DISPLAY_11x18_CLEAR_LINE, Font_11x18, White);

    for (int i = 0; i < len; i++) {
        ssd1306_SetCursor(x, y);
        char v = ssd1306_WriteChar(str[i], Font_11x18, White);
        if (v == 0) {
            ctrl->error_cb(ctrl->error_ctx,
                           (_Error_t) { .controller = CONTROLLER_DISPLAY,
                                        .peripheral = PERIPHERAL_GME,
                                        .code = ERROR_GME_INVALID_CHAR,
                                        .function = FUNCTION_GME_WRITE_CHAR });
        }
        x += Font_11x18.width + 2;
    }

    ssd1306_UpdateScreen();
}

void Display_Controller_ShowVariableChannel(Display_Controller_t *ctrl, int32_t voltage_whole,
                                            uint32_t voltage_decimal, int32_t current_whole,
                                            uint32_t current_decimal)
{
    char buf[DISPLAY_BLUE_MAX_CHAR + 1];

    snprintf(buf, sizeof(buf), "%" PRId32 ".%" PRIu32 "V", voltage_whole, voltage_decimal);
    Display_Controller_Write_Blue(ctrl, buf, DISPLAY_BLUE_TOP);

    snprintf(buf, sizeof(buf), "%" PRId32 ".%" PRIu32 "mA", current_whole, current_decimal);
    Display_Controller_Write_Blue(ctrl, buf, DISPLAY_BLUE_BOT);
}

// chan->rotary.clk_port = RTRY_CLK_GPIO_Port;
// chan->rotary.clk_pin = RTRY_CLK_Pin;
// chan->rotary.dt_port = RTRY_DT_GPIO_Port;
// chan->rotary.dt_pin = RTRY_DT_Pin;
// chan->rotary.sw_port = RTRY_SW_GPIO_Port;
// chan->rotary.sw_pin = RTRY_SW_Pin;
// chan->rotary.last_clk = HAL_GPIO_ReadPin(RTRY_CLK_GPIO_Port, RTRY_CLK_Pin);
// chan->rotary.position = 0;
// chan->rotary.mode = ROTARY_MODE_OFF;

// static int8_t rotary_read(Channel_VAR_Rotary_t *rotary)
// {
//     uint8_t clk = HAL_GPIO_ReadPin(rotary->clk_port, rotary->clk_pin);

//     if (clk != rotary->last_clk && clk == 0) {
//         uint8_t dt = HAL_GPIO_ReadPin(rotary->dt_port, rotary->dt_pin);

//         if (dt == 0) {
//             rotary->position++;
//             rotary->last_clk = clk;
//             return 1;
//         } else {
//             rotary->position--;
//             rotary->last_clk = clk;
//             return -1;
//         }
//     }

//     rotary->last_clk = clk;

//     return 0;
// }

// float pending_voltage = 0.0f;

// uint8_t Channel_VAR_PollRotary(Power_Controller_t *ctrl, Channel_VAR_t *chan)
// {
//     uint8_t err = 0;
//     int8_t change = rotary_read(&chan->rotary);

//     if (chan->rotary.pressed) {
//         chan->rotary.pressed = false;

//         switch (chan->rotary.mode) {
//         case ROTARY_MODE_OFF:
//             chan->rotary.mode = ROTARY_MODE_ADJUST;
//             pending_voltage = chan->cur_voltage;
//             printf("off to adjust mode\r\n");
//             break;

//         case ROTARY_MODE_ADJUST:
//             chan->rotary.mode = ROTARY_MODE_CONFIRM;
//             printf("adjust to confirm mode\r\n");
//             break;

//         case ROTARY_MODE_CONFIRM:
//             err = Channel_VAR_SetVoltage(ctrl, chan, pending_voltage);
//             chan->rotary.mode = ROTARY_MODE_OFF;
//             if (err != ERROR_NONE) {
//                 return err;
//             }
//             printf("confirm to off mode\r\n");
//             break;
//         }
//     }

//     printf("change: %d\r\n", change);

//     if (change != 0 && chan->rotary.mode == ROTARY_MODE_ADJUST) {
//         pending_voltage += change * 0.1f;

//         if (pending_voltage < MCP_MIN_VOLTAGE) {
//             pending_voltage = MCP_MIN_VOLTAGE;
//         }
//         if (pending_voltage > MCP_MAX_VOLTAGE) {
//             pending_voltage = MCP_MAX_VOLTAGE;
//         }
//         printf("pending_voltage: %.4f\r\n", pending_voltage);
//     }

//     if (change != 0 && chan->rotary.mode == ROTARY_MODE_CONFIRM) {
//         chan->rotary.mode = ROTARY_MODE_ADJUST;
//         printf("confirm to adjust\r\n");
//     }

//     return err;
// }