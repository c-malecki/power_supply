/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, NC_Pin|NCC14_Pin|NCC15_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, NCA0_Pin|GPIO_BUCK_VAR_Pin|GPIO_BUCK_12V_Pin|GPIO_BUCK_6V5_Pin
                          |NCA7_Pin|NCA8_Pin|NCA9_Pin|NCA15_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, NCB0_Pin|NCB1_Pin|NC_BOOT_Pin|NCB10_Pin
                          |NCB3_Pin|NCB4_Pin|GPIO_MOSFET_3V3_Pin|GPIO_MOSFET_5V_Pin
                          |GPIO_MOSFET_VAR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NC_Pin NCC14_Pin NCC15_Pin */
  GPIO_InitStruct.Pin = NC_Pin|NCC14_Pin|NCC15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : NCA0_Pin NCA7_Pin NCA8_Pin NCA9_Pin
                           NCA15_Pin */
  GPIO_InitStruct.Pin = NCA0_Pin|NCA7_Pin|NCA8_Pin|NCA9_Pin
                          |NCA15_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_BUCK_VAR_Pin GPIO_BUCK_12V_Pin GPIO_BUCK_6V5_Pin */
  GPIO_InitStruct.Pin = GPIO_BUCK_VAR_Pin|GPIO_BUCK_12V_Pin|GPIO_BUCK_6V5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : NCB0_Pin NCB1_Pin NC_BOOT_Pin NCB10_Pin
                           NCB3_Pin NCB4_Pin GPIO_MOSFET_3V3_Pin GPIO_MOSFET_5V_Pin
                           GPIO_MOSFET_VAR_Pin */
  GPIO_InitStruct.Pin = NCB0_Pin|NCB1_Pin|NC_BOOT_Pin|NCB10_Pin
                          |NCB3_Pin|NCB4_Pin|GPIO_MOSFET_3V3_Pin|GPIO_MOSFET_5V_Pin
                          |GPIO_MOSFET_VAR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_BTN_3V3_Pin GPIO_BTN_5V_Pin GPIO_BTN_VARV_Pin GPIO_BTN_DSP_Pin */
  GPIO_InitStruct.Pin = GPIO_BTN_3V3_Pin|GPIO_BTN_5V_Pin|GPIO_BTN_VARV_Pin|GPIO_BTN_DSP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : GPIO_RTRY_DT_2_Pin GPIO_RTRY_SW_2_Pin */
  GPIO_InitStruct.Pin = GPIO_RTRY_DT_2_Pin|GPIO_RTRY_SW_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : GPIO_RTRY_CLK_Pin */
  GPIO_InitStruct.Pin = GPIO_RTRY_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIO_RTRY_CLK_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
