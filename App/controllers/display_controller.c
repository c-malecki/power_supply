#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "display_controller.h"
#include "GME12864-13.h"

uint8_t Display_Controller_Init(Display_Controller_t *ctrl, I2C_HandleTypeDef *i2c_handle)
{
    uint8_t err = GME_WriteCmd(i2c_handle);
    return err;
}