/**
  ******************************************************************************
  * @file    stm32l0xx_it.c
  * @brief   Interrupt Service Routines.
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

#include "main.h"
#include "stm32l0xx_it.h"

#include "decoder.h"

extern TIM_HandleTypeDef * const tim_dcc;
extern TIM_HandleTypeDef * const tim_tick;

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/

/**
  * @brief This function handles Non maskable Interrupt.
  */
void NMI_Handler(void)
{
	while (1) {
	}
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
	while (1) {
	}
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/******************************************************************************/
/* STM32L0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line 4 to 15 interrupts.
  */
void EXTI4_15_IRQHandler(void)
{
	GPIOA->BRR |= GPIO_PIN_3;

	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);

	GPIOA->BSRR |= GPIO_PIN_3;
}


