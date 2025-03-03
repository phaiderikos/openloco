/*******************************************************************************
 * @file    :   bsp_uart.c
 * @brief   :   Board support package for the handling the UART
 * @author  :   Davide Campagna
 * @date    :   May 02, 2024
 * @version :   V1.0
 ******************************************************************************/

#include "bsp.h"
#include "qpc.h"

#define UART_TX_TIMEOUT     (100u)      /**< Uart timeout: 100 ms */

Q_DEFINE_THIS_MODULE("BSP_UART")

static UART_HandleTypeDef uart2 = {
    .Instance = USART2,
    .Init = {
        /* Baudrate = 115200 */
        .BaudRate = 115200u,
        /* 8n1 */
        .WordLength = UART_WORDLENGTH_8B,
        .Parity = UART_PARITY_NONE,
        .StopBits = UART_STOPBITS_1,
        .Mode = UART_MODE_TX,
        .HwFlowCtl = UART_HWCONTROL_NONE,
        .OverSampling = UART_OVERSAMPLING_16,
        .OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLED
    },
    .AdvancedInit = {
        .AdvFeatureInit = UART_ADVFEATURE_TXINVERT_INIT,
        .TxPinLevelInvert = UART_ADVFEATURE_TXINV_ENABLE
    }
};

/* UART structure pointers definitions and assignment ------------ */
UART_HandleTypeDef * const uart_spy = &uart2;

DMA_TypeDef * const dma_handler = DMA1;
DMA_Request_TypeDef * const dma_req_handler = DMA1_CSELR;
DMA_Channel_TypeDef * const dma_uart2_tx = DMA1_Channel4;

/**
 * @brief BSP_InitUart
 *          Initialize the uart peripheral.
 */
void BSP_InitUart(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    if (HAL_UART_Init(uart_spy) != HAL_OK) {
        Q_ERROR_ID(100);
    }

    /* Enable DMA request generation by the UART peripheral */
    uart_spy->Instance->CR3 |= USART_CR3_DMAT;

    /* Setup DMA1 for UART2 */
    /* USART2 request is #4 and Tx is connected to Ch4 */
    dma_req_handler->CSELR = DMA_REQUEST_4 << DMA_CSELR_C4S_Pos;
    /**
     * PL = 3 (very high)
     * MSIZE and PSIZE = 0 (8 bits)
     * MINC = 1
     * DIR = 1 (Read from memory)
     */
    dma_uart2_tx->CCR = DMA_CCR_PL_1 | DMA_CCR_PL_0 | DMA_CCR_MINC | DMA_CCR_DIR;   /* 0x00003090u */
    /* Peripheral register: UART Transmit Data Register */
    dma_uart2_tx->CPAR = (uint32_t)&uart_spy->Instance->TDR;

    __HAL_UART_ENABLE(uart_spy);
    uart_spy->Instance->CR1 |= USART_CR1_TE;
}

void BSP_UsartTx(uint8_t const * const tx_buff, uint32_t const size)
{
    HAL_UART_Transmit(uart_spy, tx_buff, size, UART_TX_TIMEOUT);
}

void BSP_UsartTxDMA(uint8_t const * const tx_buff, uint32_t const size)
{
    HAL_UART_Transmit_DMA(uart_spy, tx_buff, size);
}
