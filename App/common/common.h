#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdint.h>

typedef enum {
    CONTROLLER_NONE = 0,
    CONTROLLER_DISPLAY,
    CONTROLLER_INPUT,
    CONTROLLER_LED,
    CONTROLLER_POWER,
    CONTROLLER_TEMPERATURE,
} _Controllers;

extern const char *_Controller_Lookup[];

typedef enum {
    PERIPHERAL_NONE = 0,
    PERIPHERAL_MCP,
    PERIPHERAL_INA_MAIN,
    PERIPHERAL_INA_VAR,
    PERIPHERAL_GME,
    PERIPHERAL_SHT,
    PERIPHERAL_FAN,
} _Peripherals;

extern const char *_Peripheral_Lookup[];

typedef enum {
    FUNCTION_NONE = 0,
    FUNCTION_INA_PING,
    FUNCTION_MCP_PING,
    FUNCTION_GME_PING,
    FUNCTION_SHT_PING,
    FUNCTION_INA_INIT,
    FUNCTION_INA_READ_V,
    FUNCTION_INA_READ_I,
    FUNCTION_MCP_V_TO_S,
    FUNCTION_GME_WRITE_YELLOW,
    FUNCTION_GME_WRITE_BLUE,
    FUNCTION_GME_WRITE_CHAR,
    FUNCTION_SHT_INIT,
    FUNCTION_SHT_START_READ,
    FUNCTION_SHT_GET_RESULT,
    FUNCTION_TIM_IC_START_IT,
} _Functions;

extern const char *_Function_Lookup[];

typedef enum {
    ERROR_NONE = 0,
    ERROR_I2C_GENERIC_ERROR,
    ERROR_I2C_BUSY,
    ERROR_I2C_TIMEOUT,
    ERROR_POWER_OVERCURRENT,
    ERROR_POWER_OVERVOLTAGE,
    ERROR_GME_INVALID_STR_LEN,
    ERROR_GME_INVALID_CHAR,
    ERROR_UNKNOWN,
} _Error_Codes;

extern const char *_Error_Message_Lookup[];

typedef struct
{
    _Error_Codes code;
    uint8_t blinks;
} _Error_Blink_t;

extern const _Error_Blink_t _Error_Blink_Lookup[];

typedef struct
{
    _Controllers controller;
    _Peripherals peripheral;
    _Error_Codes code;
    _Functions function;
} _Error_t;

_Error_Codes ConvHALError(uint8_t hal_error);

#endif // __COMMON_H__