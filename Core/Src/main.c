/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"
#include "error.h"
#include "app.h"
#include "power_controller.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static App_t app;
App_Status_t status;
static char *app_ctrls[4] = { "App", "Display", "Power", "Temperature" };
static char *app_errs[6] = { "OK",          "I2C Error",       "I2C Busy",
                             "I2C Timeout", "PWR Overcurrent", "PWR Overvoltage" };

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t last_press_rotary = 0;
    static uint32_t last_press_3v3 = 0;
    static uint32_t last_press_5v = 0;
    static uint32_t last_press_vdc = 0;
    uint32_t now = HAL_GetTick();

    switch (GPIO_Pin) {
    case ROTARY_VAR_SW_Pin:
        if (now - last_press_rotary < 100)
            return;
        last_press_rotary = now;
        app.pwr_ctrl->chan_var->rotary.pressed = true;
        break;

    case BUTTON_ON_OFF_3V3_Pin:
    {
        if (now - last_press_3v3 < 100)
            return;
        last_press_3v3 = now;
        bool enabled = !app.pwr_ctrl->chan_3v3->output_enabled;
        Channel_VDC_EnableOutput(app.pwr_ctrl->chan_3v3, enabled);
        printf("3V3 enabled = %d\r\n", enabled);
        break;
    }

    case BUTTON_ON_OFF_5V_Pin:
    {
        if (now - last_press_5v < 100)
            return;
        last_press_5v = now;
        bool enabled = !app.pwr_ctrl->chan_5v->output_enabled;
        Channel_VDC_EnableOutput(app.pwr_ctrl->chan_5v, enabled);
        printf("5V enabled = %d\r\n", enabled);
        break;
    }

    case BUTTON_ON_OFF_VAR_Pin:
        if (now - last_press_vdc < 100)
            return;
        last_press_vdc = now;
        bool enabled = !app.pwr_ctrl->chan_var->output_enabled;
        Channel_VAR_EnableOutput(app.pwr_ctrl->chan_var, enabled);
        printf("VDC_Channel:\nenabled %u\ntarget_v %.4fV\ncur_v %.4fV\ncur_dac_steps %u\ncur_i "
               "%.4fmA\r\n\n",
               enabled, app.pwr_ctrl->chan_var->target_voltage, app.pwr_ctrl->chan_var->cur_voltage,
               app.pwr_ctrl->chan_var->cur_dac_steps, app.pwr_ctrl->chan_var->cur_current);
        break;
    }
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
    SEGGER_RTT_Write(0, ptr, len);
    return len;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    /* USER CODE BEGIN 2 */
    SEGGER_RTT_Init();

    HAL_Delay(100);

    status = App_Init(&app, &hi2c1);
    if (status.err != 0) {
        printf("App_Init:\n%s Error: %s\r\n\n", app_ctrls[status.ctrl], app_errs[status.err]);
    }

    HAL_Delay(1000);

    status = App_Dev_Test(&app);
    if (status.err != HAL_OK) {
        printf("App_Dev_Test:\nController %s Error: %s\r\n\n", app_ctrls[status.ctrl],
               app_errs[status.err]);
    }

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */

    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */

        // HAL_Delay(100);
        // err = VDC_Rotary_Poll(app.pwr_ctrl->chan_var, app.i2c_handle);
        // if (err != 0) {
        //     printf("%s Error: %d; %s\r\n", app_ctrls[status.ctrl], status.err,
        //            app_errs[status.err]);
        // }
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 96;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) { }
    /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
