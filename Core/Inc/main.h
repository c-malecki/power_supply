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
#define BUTTON_TOGGLE_3V3_Pin GPIO_PIN_1
#define BUTTON_TOGGLE_3V3_GPIO_Port GPIOA
#define BUTTON_TOGGLE_3V3_EXTI_IRQn EXTI1_IRQn
#define BUTTON_TOGGLE_5V_Pin GPIO_PIN_2
#define BUTTON_TOGGLE_5V_GPIO_Port GPIOA
#define BUTTON_TOGGLE_5V_EXTI_IRQn EXTI2_IRQn
#define BUTTON_TOGGLE_VAR_Pin GPIO_PIN_3
#define BUTTON_TOGGLE_VAR_GPIO_Port GPIOA
#define BUTTON_TOGGLE_VAR_EXTI_IRQn EXTI3_IRQn
#define ROTARY_CLK_Pin GPIO_PIN_4
#define ROTARY_CLK_GPIO_Port GPIOA
#define ROTARY_DT_Pin GPIO_PIN_5
#define ROTARY_DT_GPIO_Port GPIOA
#define ROTARY_SW_Pin GPIO_PIN_6
#define ROTARY_SW_GPIO_Port GPIOA
#define ROTARY_SW_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON_MENU_Pin GPIO_PIN_7
#define BUTTON_MENU_GPIO_Port GPIOA
#define BUTTON_MENU_EXTI_IRQn EXTI9_5_IRQn
#define PWM_FAN_Pin GPIO_PIN_0
#define PWM_FAN_GPIO_Port GPIOB
#define TEMP_ANALOG_Pin GPIO_PIN_1
#define TEMP_ANALOG_GPIO_Port GPIOB
#define TEMP_DIGITAL_Pin GPIO_PIN_10
#define TEMP_DIGITAL_GPIO_Port GPIOB
#define MOSFET_3V3_Pin GPIO_PIN_8
#define MOSFET_3V3_GPIO_Port GPIOA
#define MOSFET_5V_Pin GPIO_PIN_9
#define MOSFET_5V_GPIO_Port GPIOA
#define MOSFET_VAR_Pin GPIO_PIN_10
#define MOSFET_VAR_GPIO_Port GPIOA
#define LED_3V3_Pin GPIO_PIN_4
#define LED_3V3_GPIO_Port GPIOB
#define LED_5V_Pin GPIO_PIN_5
#define LED_5V_GPIO_Port GPIOB
#define LED_VAR_Pin GPIO_PIN_6
#define LED_VAR_GPIO_Port GPIOB
#define LED_STATUS_Pin GPIO_PIN_7
#define LED_STATUS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
