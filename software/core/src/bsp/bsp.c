/*******************************************************************************
 * @file    :   bsp.c
 * @brief   :   Board support package for the decoder
 * @author  :   Davide Campagna
 * @date    :   May 02, 2024
 * @version :   V1.0
 ******************************************************************************/
#include "bsp.h"

Q_DEFINE_THIS_MODULE("BSP")

#define SIMPLE_DELAY_2MS    ((uint32_t)2 * 2e6 / 20)    /* 2 multiplied by 2M divided by 20 (approximate instructions), since MSI clock is about ~2MHz*/

#ifdef Q_SPY
/* QSpy Tx buffer */
static uint8_t qspy_tx[256u];

/* Local QSpy source IDs */
static QSpyId const SysTickISR = {0};
#endif

static void configure_clock(void);

/**
 * BSP_Init
 * @brief Initialise all MCU peripherals
 */
void BSP_Init(void)
{
    /**
     * HAL_Init:
     * - reset all the enabled peripherals
     * - initialise SysTick timer based on clock frequency
     * - setup the interface for the Flash memory
     */
    HAL_Init();

    /* Configure the system clock */
    configure_clock();

    BSP_InitGPIOs();
    BSP_InitTimers();
    BSP_InitUart();

    /* EXTI interrupt init (DCC_DATA is pin9) */
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);

    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

#ifdef Q_SPY
    /* ---- QSpy -------------------------------------------------------- */
    if (QS_INIT(NULL) != 1) {
        Q_ERROR_ID(100);
    }

    /* QSpy dictionaries */
    QS_OBJ_DICTIONARY(&SysTickISR);

    /* QSpy filters */
    QS_GLB_FILTER(QS_ALL_RECORDS);          /* All records*/
    /* TODO: Filter records!!! */
    QS_GLB_FILTER(-QS_QF_TICK);

    QS_GLB_FILTER(-QS_QEP_STATE_ENTRY);     /**< a state was entered                            */
    QS_GLB_FILTER(-QS_QEP_STATE_EXIT);      /**< a state was exited                             */
    QS_GLB_FILTER(-QS_QEP_STATE_INIT);      /**< an initial transition was taken in a state     */
    QS_GLB_FILTER(QS_QEP_INIT_TRAN);        /**< the top-most initial transition was taken      */
    QS_GLB_FILTER(-QS_QEP_INTERN_TRAN);     /**< an internal transition was taken               */
    QS_GLB_FILTER(QS_QEP_TRAN);             /**< a regular transition was taken                 */
    QS_GLB_FILTER(-QS_QEP_IGNORED);         /**< an event was ignored (silently discarded)      */
    QS_GLB_FILTER(-QS_QEP_DISPATCH);        /**< an event was dispatched (begin of RTC step)    */
    QS_GLB_FILTER(QS_QEP_UNHANDLED);        /**< an event was un-handled due to a guard         */
#endif
}

/* ======== QF FUNCTIONS DEFINITIONS ======================================== */

void QF_onStartup(void)
{
    /* TODO: add startup code here */
}

void QV_onIdle(void)
{
    /* NOTE: QV_onIdle() is invoked with interrupts **DISABLED** */

    /* Toggle C_GPIOA_Pin */
#if 0
    if (GPIOA->ODR & GPIO_PIN_3) {
        GPIOA->BRR = GPIO_PIN_3;
    }
    else {
        GPIOA->BSRR = GPIO_PIN_3;
    }
#endif

    /* At last, enable interrupts */
    QF_INT_ENABLE();

    /* TODO: Add QSpy tx handling */
#ifdef Q_SPY

    uint16_t length = 32u;          /* Avoid too long block of data */
    uint8_t const * block = NULL;

    /* Reset TCFLAGS (if any ) */
    if ((dma_handler->ISR
            & (DMA_IFCR_CGIF4_Msk | DMA_IFCR_CTCIF4_Msk | DMA_IFCR_CHTIF4_Msk | DMA_IFCR_CTEIF4_Msk)) != 0) {

        dma_handler->IFCR = DMA_IFCR_CGIF4_Msk \
                    | DMA_IFCR_CTCIF4_Msk \
                    | DMA_IFCR_CHTIF4_Msk \
                    | DMA_IFCR_CTEIF4_Msk;

        /* Disable the channel after a successful transmission */
        dma_uart2_tx->CCR &= ~DMA_CCR_EN;
    }

    QF_INT_DISABLE();
    if ((dma_uart2_tx->CCR & DMA_CCR_EN_Msk) == 0u) {
        /* dma_uart2_tx is ready */
        block = QS_getBlock(&length);
    }
    QF_INT_ENABLE();

    if ((block != NULL) && (length != 0u)) {
        /* Connect the address of the block to the DMA */
        dma_uart2_tx->CMAR = (uint32_t) block;
        /* Set the transfer size */
        dma_uart2_tx->CNDTR = length;
        /* Enable DMA transfer */
        dma_uart2_tx->CCR |= DMA_CCR_EN;
    }
#endif
}

Q_NORETURN Q_onError(char const *module, int_t location)
{
    QS_ASSERTION(module, location, 10000U);

    /* TODO: Shut down the motor and reset all outputs */

    for (;;) {
        /* Never exit the loop */
        /* NOTE: Restart instead of hang? */
    }
}

#ifdef Q_SPY
/* ======== Q_SPY =========================================================== */
uint8_t QS_onStartup(void const *arg)
{
    Q_UNUSED_PAR(arg);

    QS_initBuf(qspy_tx, sizeof(qspy_tx));

    return 1u;
}

QSTimeCtr QS_onGetTime(void)
{
    return tim_dcc->Instance->CNT;
}

/* Unused function */
void QS_onCleanup(void)
{
    /* Unused function. Body is indeed empty */
}


/**
 * QS_onFlush
 * @brief Flush the QSpy Tx queue
 */
void QS_onFlush(void)
{
    uint16_t b;

    do {
        /* TODO: Use DMA instead to allow for much higher throughput */
        b = QS_getByte();

        if (b != QS_EOD) {
            while (!__HAL_UART_GET_FLAG(uart_spy, UART_FLAG_TXE)) {
                /* Wait until Tx is completed */
            }
            uart_spy->Instance->TDR = b;
        }
    } while (b != QS_EOD);
}
#endif

/* ======== ISRs ============================================================ */

/**
 * @brief SysTick_Handler
 *          SysTick ISR
 */
void SysTick_Handler(void)
{
    /* Invoke the system clock tick processing for tick rate 0 */
    QTIMEEVT_TICK_X(0u, &SysTickISR);

    /* NOTE: HAL ticks are not being incremented, blocking HAL functions should
     *    not be called*/

    input_sample();
}

/* ======== Private function declarations =================================== */

static void configure_clock(void)
{
    /**
     *
     * HSI = 16 MHz
     * PLL Source Mux = HSI
     *  PLLMul = x4
     *  PLLDiv = /2
     * PLLCLK = 32 MHz
     *
     * System Clock Mux <- PLLCLK
     *
     * SYSCLK = 32MHz
     * AHB Prescaler = /1
     *
     * HCLK = 32 MHz
     * Cortex div = /1
     * APB1 Clock Divider = 1
     * PCLK1
     *
     * 32 MHz requirements:
     *  - 1 FLASH wait
     *  - Highest voltage scale
     *
     * USART2 Source Mux = PCLK1 -> 32 MHz
     */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Calibrate HSI clock with default value ... */
    __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_HSICALIBRATION_DEFAULT);

    /* ... and enable it */
    RCC->CR |= (RCC_CR_HSION);

    /* Configure the PLL to get 32MHz from HSI: 16 MHz * 4 / 2 = 32 MHz ... */
    __HAL_RCC_PLL_CONFIG(RCC_PLLSOURCE_HSI, RCC_PLLMUL_4, RCC_PLLDIV_2);

    /* ... and enable it */
    __HAL_RCC_PLL_ENABLE();

    /* Ensure the PLL was activated */
    /* Right now there are no timing sources available, wait in a simple way */
    volatile uint32_t i;
    for (i = 0u;
            (i < (SIMPLE_DELAY_2MS))
                    && ((__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)) == 0u); i++) {
        /* Wait PLL Ready bit ... */
    }
    if (i >= (SIMPLE_DELAY_2MS)) {
        /* TIMEOUT! */
        Q_ERROR_ID(200);
    }

    /* At 32 MHz, flash latency is 1 clock cycle */
    __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);

    /* Ensure the Flash latency was changed */
    /* Right now there are no timing sources available, wait in a simple way */
    for (i = 0u;
            (i < (SIMPLE_DELAY_2MS))
                    && (__HAL_FLASH_GET_LATENCY() != FLASH_LATENCY_1); i++) {
        /* Wait PLL Ready bit ... */
    }
    if (i >= (SIMPLE_DELAY_2MS)) {
        /* TIMEOUT! */
        Q_ERROR_ID(210);
    }

    /* AHB clock is SYSCLK divided by 1 */
    RCC->CFGR |= (RCC_CFGR_HPRE_0);

    /* Select PLL as source for SYSCLK */
    RCC->CFGR |= (RCC_SYSCLKSOURCE_PLLCLK);

    /* Ensure the PLL was selected as source */
    /* Right now there are no timing sources available, wait in a simple way */
    for (i = 0u;
            (i < (SIMPLE_DELAY_2MS))
                    && (__HAL_RCC_GET_SYSCLK_SOURCE()
                            != RCC_SYSCLKSOURCE_STATUS_PLLCLK); i++) {
        /* Wait switch status bit ... */
    }
    if (i >= (SIMPLE_DELAY_2MS)) {
        /* TIMEOUT! */
        Q_ERROR_ID(220);
    }

    /* PCLK1 (APB1 low speed) and PCLK2 (APB2 low speed) connected to HCLK divided by 1 */
    RCC->CFGR |= RCC_CFGR_PPRE1_0;
    RCC->CFGR |= RCC_CFGR_PPRE2_0;

    /* Setup ticks: 32MHz / 32000 = 1KHz */
    SysTick_Config(32000);

    /* Tell the HAL the system clock speed (in MHz) */
    SystemCoreClock = 32e6;
}
