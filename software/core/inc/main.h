/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

#ifndef __MAIN_H
#define __MAIN_H

#ifndef STM32L031xx
#define STM32L031xx
#endif

#include "stm32l0xx_hal.h"

void Error_Handler(void);

/* Front headlight - PluX16 pin 7 */
#define C_FOF_Pin GPIO_PIN_0
#define C_FOF_GPIO_Port GPIOA
/* Rear headlight - PluX16 pin 13 */
#define C_FOR_Pin GPIO_PIN_1
#define C_FOR_GPIO_Port GPIOA
/* PluX16 pins 3 and 4*/
#define C_GPIOB_Pin GPIO_PIN_2
#define C_GPIOB_GPIO_Port GPIOA
#define C_GPIOA_Pin GPIO_PIN_3
#define C_GPIOA_GPIO_Port GPIOA
/* DRV8872 control pins */
#define nFAULT_Pin GPIO_PIN_5
#define nFAULT_GPIO_Port GPIOA
#define IN_1_Pin GPIO_PIN_6
#define IN_1_GPIO_Port GPIOA
#define IN_2_Pin GPIO_PIN_7
#define IN_2_GPIO_Port GPIOA
/* PluX16 pins 16 and 18 */
#define C_AUX1_Pin GPIO_PIN_0
#define C_AUX1_GPIO_Port GPIOB
#define C_AUX2_Pin GPIO_PIN_1
#define C_AUX2_GPIO_Port GPIOB
/* DCC signal coming from the tracks */
#define DCC_DATA_Pin GPIO_PIN_9
#define DCC_DATA_GPIO_Port GPIOA

#endif /* __MAIN_H */
