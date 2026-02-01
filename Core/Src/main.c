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
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include <stdint.h>
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "SEGGER_RTT.h"
#include "error.h"
#include "INA219.h"
#include "MCP4725.h"
#include "OLED.h"
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

// uint32_t last_interrupt_time[4] = { 0 };
// #define DEBOUNCE_DELAY 200

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

// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
// {
//     uint16_t led_pin;
//     GPIO_TypeDef *btn_port;

//     if (GPIO_Pin == BTN_RED_Pin) {
//         led_pin = LED_RED_Pin;
//         btn_port = GPIOB;
//     } else if (GPIO_Pin == BTN_GREEN_Pin) {
//         led_pin = LED_GREEN_Pin;
//         btn_port = GPIOB;
//     } else if (GPIO_Pin == BTN_BLUE_Pin) {
//         led_pin = LED_BLUE_Pin;
//         btn_port = GPIOB;
//     } else if (GPIO_Pin == BTN_YELLOW_Pin) {
//         led_pin = LED_YELLOW_Pin;
//         btn_port = GPIOB;
//     } else {
//         return;
//     }

//     GPIO_PinState btn_state = HAL_GPIO_ReadPin(btn_port, GPIO_Pin);

//     if (btn_state == GPIO_PIN_RESET) {
//         HAL_GPIO_WritePin(GPIOA, led_pin, GPIO_PIN_SET);
//     } else {
//         HAL_GPIO_WritePin(GPIOA, led_pin, GPIO_PIN_RESET);
//     }
// }

/**
 * @brief  Values were with ST-Link connected which reduces V slightly
 *
 *         ΔV = 1.38 - 1068 = -9.3V
 *         ΔDAC = 4095 - 511 = 3584 steps
 *
 *         Slope = -9.3 / 3584 = -0.002595 V/step (actual 0.0025948661)
 *
 *    *** 0 = ~12.00V
 *       511 = ~10.68
 *       1023 = ~9.29
 *   *** 1133 = ~9.05V
 *       1534 = ~7.91
 *       2047 = ~6.52
 *       2559 = ~5.14
 *       3071 = ~3.75
 *   *** 3236 = ~3.333
 *       3583 = ~2.36
 *       4095 = ~1.38
 *
 *        +/-100 to DAC = +/-0.2595V
 *
 * @retval HAL status
 */
static void DAC_Sweep_Range(void)
{
    uint16_t range[4] = { DAC_VAL_12V, DAC_VAL_9V, DAC_VAL_5V, DAC_VAL_3V3 };

    for (int i = 0; i < 4; i++) {
        HAL_StatusTypeDef err = MCP_Write_Value(&hi2c1, range[i]);
        if (err != HAL_OK) {
            printf("DAC_Sweep_Range: Error: %d\r\n", err);
            return;
        }

        double current = INA_Read_Current(&hi2c1);
        double voltage = INA_Read_Voltage(&hi2c1);
        double power = voltage * current;

        printf("V: %.2fV | I: %.2fmA | P: %.2fmW\n", voltage, current, power);

        HAL_Delay(3000);
    }
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

    // SEGGER_RTT_Init();

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_I2C1_Init();
    /* USER CODE BEGIN 2 */

    HAL_Delay(100);

    HAL_StatusTypeDef err = INA_Init(&hi2c1);
    if (err != HAL_OK) {
        printf("INA_Init: Error: %d\r\n", err);
    }

    err = MCP_Write_Value(&hi2c1, DAC_VAL_12V);
    if (err != HAL_OK) {
        printf("MCP_Write_Value: Error: %d\r\n", err);
    }

    HAL_Delay(200);

    OLED_Write_Cmd(&hi2c1);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        DAC_Sweep_Range();
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
