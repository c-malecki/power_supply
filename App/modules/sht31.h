#ifndef __SHT31_H__
#define __SHT31_H__

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "common.h"

#define SHT_I2C_ADDRESS (0x44 << 1)

#define SHT_CMD_MEASURE_MSB (0x24)
#define SHT_CMD_MEASURE_LSB (0x00)
#define SHT_CMD_SOFT_RESET_MSB (0x30)
#define SHT_CMD_SOFT_RESET_LSB (0xA2)

typedef struct
{
    _Error_Codes code;
    int32_t temp_c;
} SHT_Result_t;

_Error_Codes SHT_Init(I2C_HandleTypeDef *i2c_handle);
_Error_Codes SHT_StartRead(I2C_HandleTypeDef *i2c_handle);
SHT_Result_t SHT_GetResult(I2C_HandleTypeDef *i2c_handle);

#endif // __SHT31_H__