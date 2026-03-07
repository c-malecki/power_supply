#include "stm32f411xe.h"
#include <stdint.h>
#include <stdbool.h>
#include "display_controller.h"
#include "stm32f4xx_hal.h"
#include "tim.h"
#include "ssd1306_conf.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "common.h"

_Error_Codes Display_Controller_PingPeripherals(I2C_HandleTypeDef *i2c_handle)
{
    return ConvHALError(HAL_I2C_IsDeviceReady(i2c_handle, SSD1306_I2C_ADDR, 3, 100));
}

void Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    char *text = "Initializing...";
    ssd1306_Init();
    ssd1306_SetCursor(12, 27);
    ssd1306_WriteString(text, Font_7x10, White);
    ssd1306_UpdateScreen();
}

void Display_Controller_ShowVariableChannel(int32_t voltage_whole, uint32_t voltage_decimal,
                                            int32_t current_whole, uint32_t current_decimal)
{
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