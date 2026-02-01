#include "error.h"
#include <stdio.h>

HAL_StatusTypeDef Check_I2C_Err(HAL_StatusTypeDef status)
{
    // HAL_OK 0x00U
    // HAL_ERROR 0x01U
    // HAL_BUSY 0x02U
    // HAL_TIMEOUT 0x03U

    if (status != HAL_OK) {
        printf("I2C Error: 0x%02X\r\n", status);

        switch (status) {
        case HAL_ERROR:
            printf("HAL_ERROR\r\n");
            break;
        case HAL_BUSY:
            printf("HAL_BUSY\r\n");
            break;
        case HAL_TIMEOUT:
            printf("HAL_TIMEOUT\r\n");
            break;
        default:
            printf("Unknown error\r\n");
            break;
        }
        // HAL_NVIC_SystemReset();
    }

    return status;
}