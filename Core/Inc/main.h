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
#define BTN_VDC_PRE1_Pin GPIO_PIN_1
#define BTN_VDC_PRE1_GPIO_Port GPIOA
#define BTN_VDC_PRE1_EXTI_IRQn EXTI1_IRQn
#define BTN_VDC_PRE2_Pin GPIO_PIN_2
#define BTN_VDC_PRE2_GPIO_Port GPIOA
#define BTN_VDC_PRE2_EXTI_IRQn EXTI2_IRQn
#define BTN_VDC_PRE3_Pin GPIO_PIN_3
#define BTN_VDC_PRE3_GPIO_Port GPIOA
#define BTN_VDC_PRE3_EXTI_IRQn EXTI3_IRQn
#define ROT_CLK_Pin GPIO_PIN_4
#define ROT_CLK_GPIO_Port GPIOA
#define ROT_DT_Pin GPIO_PIN_5
#define ROT_DT_GPIO_Port GPIOA
#define ROT_SW_Pin GPIO_PIN_6
#define ROT_SW_GPIO_Port GPIOA
#define ROT_SW_EXTI_IRQn EXTI9_5_IRQn
#define MOSFET_3V3_Pin GPIO_PIN_8
#define MOSFET_3V3_GPIO_Port GPIOA
#define MOSFET_5V_Pin GPIO_PIN_9
#define MOSFET_5V_GPIO_Port GPIOA
#define MOSFET_VDC_Pin GPIO_PIN_10
#define MOSFET_VDC_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
