#include "GME12864-13.h"

uint8_t OLED_WriteCmd(I2C_HandleTypeDef *handle)
{
    uint8_t pData = OLED_DSP_OFF;
    return HAL_I2C_Mem_Write(handle, OLED_I2C_ADDR, 0x00, 1, &pData, 1, 100);
}

// The 0x00 is the memory/register address sent to the display
// indicating command mode (D/C# = 0).
// The 1 after it is the size of that register address byte.
uint8_t OLED_SetContrast(I2C_HandleTypeDef *handle)
{
    // value range 0x00–0xFF
    uint8_t pData[2] = { OLED_DSP_CONTRAST, 0xFF };
    return HAL_I2C_Mem_Write(handle, OLED_I2C_ADDR, 0x00, 1, pData, 2, 100);
}