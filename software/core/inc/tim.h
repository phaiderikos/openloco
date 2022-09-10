/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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

#ifndef __TIM_H
#define __TIM_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim22;

void MX_TIM2_Init(void);

void MX_TIM22_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

#endif /* __TIM_H */
