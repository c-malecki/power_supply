/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define GPIO_BTN_3V3_Pin GPIO_PIN_13
#define GPIO_BTN_3V3_GPIO_Port GPIOC
#define GPIO_BTN_3V3_EXTI_IRQn EXTI15_10_IRQn
#define GPIO_BTN_5V_Pin GPIO_PIN_14
#define GPIO_BTN_5V_GPIO_Port GPIOC
#define GPIO_BTN_5V_EXTI_IRQn EXTI15_10_IRQn
#define GPIO_BTN_VAR_Pin GPIO_PIN_15
#define GPIO_BTN_VAR_GPIO_Port GPIOC
#define GPIO_BTN_VAR_EXTI_IRQn EXTI15_10_IRQn
#define PWM_FAN_Pin GPIO_PIN_0
#define PWM_FAN_GPIO_Port GPIOA
#define FAN_TACH_Pin GPIO_PIN_1
#define FAN_TACH_GPIO_Port GPIOA
#define BUCK_VAR_Pin GPIO_PIN_2
#define BUCK_VAR_GPIO_Port GPIOA
#define BUCK_12V_Pin GPIO_PIN_3
#define BUCK_12V_GPIO_Port GPIOA
#define BUCK_6V5_Pin GPIO_PIN_4
#define BUCK_6V5_GPIO_Port GPIOA
#define GPIO_RTRY_SW_Pin GPIO_PIN_5
#define GPIO_RTRY_SW_GPIO_Port GPIOA
#define GPIO_RTRY_DT_Pin GPIO_PIN_6
#define GPIO_RTRY_DT_GPIO_Port GPIOA
#define GPIO_RTRY_DT_EXTI_IRQn EXTI9_5_IRQn
#define GPIO_RTRY_CLK_Pin GPIO_PIN_7
#define GPIO_RTRY_CLK_GPIO_Port GPIOA
#define NC_Pin GPIO_PIN_0
#define NC_GPIO_Port GPIOB
#define NCB1_Pin GPIO_PIN_1
#define NCB1_GPIO_Port GPIOB
#define NC_BOOT_Pin GPIO_PIN_2
#define NC_BOOT_GPIO_Port GPIOB
#define NCB10_Pin GPIO_PIN_10
#define NCB10_GPIO_Port GPIOB
#define GPIO_BTN_MENU_Pin GPIO_PIN_12
#define GPIO_BTN_MENU_GPIO_Port GPIOB
#define GPIO_BTN_MENU_EXTI_IRQn EXTI15_10_IRQn
#define NCB13_Pin GPIO_PIN_13
#define NCB13_GPIO_Port GPIOB
#define NCB14_Pin GPIO_PIN_14
#define NCB14_GPIO_Port GPIOB
#define NCB15_Pin GPIO_PIN_15
#define NCB15_GPIO_Port GPIOB
#define NCA8_Pin GPIO_PIN_8
#define NCA8_GPIO_Port GPIOA
#define NCA9_Pin GPIO_PIN_9
#define NCA9_GPIO_Port GPIOA
#define NCA10_Pin GPIO_PIN_10
#define NCA10_GPIO_Port GPIOA
#define NCA11_Pin GPIO_PIN_11
#define NCA11_GPIO_Port GPIOA
#define NCA12_Pin GPIO_PIN_12
#define NCA12_GPIO_Port GPIOA
#define NCA15_Pin GPIO_PIN_15
#define NCA15_GPIO_Port GPIOA
#define NCB3_Pin GPIO_PIN_3
#define NCB3_GPIO_Port GPIOB
#define PWM_LED_Pin GPIO_PIN_4
#define PWM_LED_GPIO_Port GPIOB
#define GPIO_MOSFET_3V3_Pin GPIO_PIN_5
#define GPIO_MOSFET_3V3_GPIO_Port GPIOB
#define GPIO_MOSFET_5V_Pin GPIO_PIN_6
#define GPIO_MOSFET_5V_GPIO_Port GPIOB
#define GPIO_MOSFET_VAR_Pin GPIO_PIN_7
#define GPIO_MOSFET_VAR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
