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
#define BTN_CHAN_3V3_Pin GPIO_PIN_13
#define BTN_CHAN_3V3_GPIO_Port GPIOC
#define BTN_CHAN_3V3_EXTI_IRQn EXTI15_10_IRQn
#define BTN_CHAN_5V_Pin GPIO_PIN_14
#define BTN_CHAN_5V_GPIO_Port GPIOC
#define BTN_CHAN_5V_EXTI_IRQn EXTI15_10_IRQn
#define BTN_CHAN_VAR_Pin GPIO_PIN_15
#define BTN_CHAN_VAR_GPIO_Port GPIOC
#define BTN_CHAN_VAR_EXTI_IRQn EXTI15_10_IRQn
#define RELAY_CHAN_PWR_Pin GPIO_PIN_1
#define RELAY_CHAN_PWR_GPIO_Port GPIOA
#define PWM_CHAN_3V3_LED_B_Pin GPIO_PIN_2
#define PWM_CHAN_3V3_LED_B_GPIO_Port GPIOA
#define PWM_CHAN_3V3_LED_G_Pin GPIO_PIN_3
#define PWM_CHAN_3V3_LED_G_GPIO_Port GPIOA
#define BTN_DSP_MENU_Pin GPIO_PIN_4
#define BTN_DSP_MENU_GPIO_Port GPIOA
#define BTN_DSP_MENU_EXTI_IRQn EXTI4_IRQn
#define RTRY_SW_Pin GPIO_PIN_5
#define RTRY_SW_GPIO_Port GPIOA
#define RTRY_DT_Pin GPIO_PIN_6
#define RTRY_DT_GPIO_Port GPIOA
#define RTRY_DT_EXTI_IRQn EXTI9_5_IRQn
#define RTRY_CLK_Pin GPIO_PIN_7
#define RTRY_CLK_GPIO_Port GPIOA
#define DS18B20_S_Pin GPIO_PIN_1
#define DS18B20_S_GPIO_Port GPIOB
#define PWM_CHAN_3V3_LED_R_Pin GPIO_PIN_10
#define PWM_CHAN_3V3_LED_R_GPIO_Port GPIOB
#define MOSFET_CHAN_3V3_Pin GPIO_PIN_13
#define MOSFET_CHAN_3V3_GPIO_Port GPIOB
#define MOSFET_CHAN_5V_Pin GPIO_PIN_14
#define MOSFET_CHAN_5V_GPIO_Port GPIOB
#define MOSFET_CHAN_VAR_Pin GPIO_PIN_15
#define MOSFET_CHAN_VAR_GPIO_Port GPIOB
#define PWM_LED_STATUS_G_Pin GPIO_PIN_8
#define PWM_LED_STATUS_G_GPIO_Port GPIOA
#define PWM_LED_STATUS_R_Pin GPIO_PIN_9
#define PWM_LED_STATUS_R_GPIO_Port GPIOA
#define PWM_LED_STATUS_B_Pin GPIO_PIN_10
#define PWM_LED_STATUS_B_GPIO_Port GPIOA
#define PWM_CHAN_5V_LED_R_Pin GPIO_PIN_15
#define PWM_CHAN_5V_LED_R_GPIO_Port GPIOA
#define PWM_CHAN_5V_LED_G_Pin GPIO_PIN_3
#define PWM_CHAN_5V_LED_G_GPIO_Port GPIOB
#define PWM_CHAN_5V_LED_B_Pin GPIO_PIN_4
#define PWM_CHAN_5V_LED_B_GPIO_Port GPIOB
#define PWM_CHAN_VAR_LED_R_Pin GPIO_PIN_5
#define PWM_CHAN_VAR_LED_R_GPIO_Port GPIOB
#define PWM_CHAN_VAR_LED_G_Pin GPIO_PIN_6
#define PWM_CHAN_VAR_LED_G_GPIO_Port GPIOB
#define PWM_CHAN_VAR_LED_B_Pin GPIO_PIN_7
#define PWM_CHAN_VAR_LED_B_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
