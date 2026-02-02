#include "GME12864-13.h"

HAL_StatusTypeDef OLED_WriteCmd(I2C_HandleTypeDef *handle)
{
    uint8_t pData = 0xAE;
    return HAL_I2C_Mem_Write(handle, OLED_I2C_ADDR, 0x00, 1, &pData, 1, 100);
}