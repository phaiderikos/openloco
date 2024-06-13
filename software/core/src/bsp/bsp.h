

#ifndef __BSP_H__
#define __BSP_H__

#include "stm32l0xx_hal.h"
#include "stdbool.h"

/* ================ Public pointers to peripheral handles ================ */
/* Timers structures declarations */
extern TIM_HandleTypeDef * const tim_dcc;
extern TIM_HandleTypeDef * const tim_tick;
extern TIM_HandleTypeDef * const tim_motor;

/* Uart structure declaration */
extern UART_HandleTypeDef * const uart_spy;

extern DMA_TypeDef * const dma_handler;
extern DMA_Request_TypeDef * const dma_req_handler ;
extern DMA_Channel_TypeDef * const dma_uart2_tx;

/* ================ Public function declarations ================ */
/* Initialisation functions */
void BSP_Init(void);
void BSP_InitTimers(void);
void BSP_InitGPIOs(void);
void BSP_InitUart(void);

/* Common helper functions */
uint32_t BSP_GetDCCTimer(bool * const ovf);
void BSP_UsartTx(uint8_t const * const tx_buff, uint32_t const size);

/* Application specific functions */
extern void BSP_SetCFOF_pin(GPIO_PinState const state);
extern void BSP_SetCFOR_pin(GPIO_PinState const state);
extern void BSP_SetAUX1_pin(GPIO_PinState const state);
extern void BSP_SetAUX2_pin(GPIO_PinState const state);
extern void BSP_SetGPIOA_pin(GPIO_PinState const state);
extern void BSP_SetGPIOB_pin(GPIO_PinState const state);

extern void BSP_RegisterDCCExtIIsr(void (*isr)(void));
extern void BSP_RegisterDCCTimIsr(void (*isr)(void));

extern void BSP_StartDCCTimer(void);

extern void BSP_StopDCCTimer(void);

#endif	/* __BPS_H__ */
