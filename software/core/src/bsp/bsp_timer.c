/*******************************************************************************
 * @file    :   bsp_timer.c
 * @brief   :   Board support package for the handling the TIM peripherals
 * @author  :   Davide Campagna
 * @date    :   Feb 28, 2025
 * @version :   V1.0
 ******************************************************************************/

#include "bsp.h"
#include "qpc.h"

Q_DEFINE_THIS_MODULE("BSP_TIMER")

static void DCC_TIM_Init(void);
static void Tick_TIM_Init(void);
static void MotorPWM_TIM_Init(void);

static void (*dcc_tim_isr)(void);

/* DCC decoder Timer */
static TIM_HandleTypeDef htim2 = {
    .Instance = TIM2,
    .Init = {
        .Prescaler = 8u - 1u,   /* 4MHz @ 32MHz clock */
        .Period = 65535u,       /* T = 16.384ms */
        .CounterMode = TIM_COUNTERMODE_UP,
        .ClockDivision = TIM_CLOCKDIVISION_DIV1,
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE
    }
};

/* Tick timer */
static TIM_HandleTypeDef htim21 = {
    .Instance = TIM21,
    .Init = {
        .Prescaler = 32u - 1u,  /* 1MHz @ 32MHz clock */
        .Period = 1000u - 1u,   /* f = 1kHz, T = 1ms */
        .CounterMode = TIM_COUNTERMODE_UP,
        .ClockDivision = TIM_CLOCKDIVISION_DIV1,
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE
    }
};

/* Motor PWM timer */
static TIM_HandleTypeDef htim22 = {
    .Instance = TIM22,
    .Init = {
        .Prescaler = 0u,        /* 32MHz @ 32MHz clock */
        .Period = 1024u - 1u,   /* f = 32kHz, T = 31.25 ms -> this ensures a 10 bit granularity for the duty cycle */
        .CounterMode = TIM_COUNTERMODE_UP,
        .ClockDivision = TIM_CLOCKDIVISION_DIV1,
        .AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE
    }
};

static TIM_OC_InitTypeDef pwm_tim_cfg = {
    .OCMode = TIM_OCMODE_PWM1,
    .Pulse = 0,
    .OCPolarity = TIM_OCPOLARITY_HIGH,
    .OCFastMode = TIM_OCFAST_DISABLE
};

static TIM_MasterConfigTypeDef tim_mstr_cfg = {
    .MasterOutputTrigger = TIM_TRGO_RESET,
    .MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE
};

static TIM_ClockConfigTypeDef tim_clk_cfg = {
    .ClockSource = TIM_CLOCKSOURCE_INTERNAL
};

/* Timer structure pointers definitions and assignment ---------------------- */
TIM_HandleTypeDef * const tim_dcc = &htim2;
TIM_HandleTypeDef * const tim_tick = &htim21;
TIM_HandleTypeDef * const tim_motor = &htim22;


/**
 * @brief TIM2 init function
 *          Initialise the timer related to the DCC signal acquisition
 */
static void DCC_TIM_Init(void)
{
    if (HAL_TIM_Base_Init(tim_dcc) != HAL_OK) {
        Q_ERROR_ID(100);
    }

    if (HAL_TIM_ConfigClockSource(tim_dcc, &tim_clk_cfg) != HAL_OK) {
        Q_ERROR_ID(110);
    }

    if (HAL_TIMEx_MasterConfigSynchronization(tim_dcc, &tim_mstr_cfg) != HAL_OK) {
        Q_ERROR_ID(120);
    }
}

/**
 * @brief TIM21 init function
 *          Initialise the timer used for Q_TICK
 */
static void Tick_TIM_Init(void)
{
    if (HAL_TIM_Base_Init(tim_tick) != HAL_OK) {
        Q_ERROR_ID(200);
    }

    if (HAL_TIM_ConfigClockSource(tim_tick, &tim_clk_cfg) != HAL_OK) {
        Q_ERROR_ID(210);
    }

    if (HAL_TIMEx_MasterConfigSynchronization(tim_tick, &tim_mstr_cfg) != HAL_OK) {
        Q_ERROR_ID(220);
    }
}

/**
 * @brief TIM22 init function
 *          Initialise the timer used for PWM generation to control the train motor.
 */
static void MotorPWM_TIM_Init(void)
{
    if (HAL_TIM_Base_Init(tim_motor) != HAL_OK) {
        Q_ERROR_ID(300);
    }

    if (HAL_TIM_ConfigClockSource(tim_motor, &tim_clk_cfg) != HAL_OK) {
        Q_ERROR_ID(310);
    }

    if (HAL_TIM_PWM_Init(tim_motor) != HAL_OK) {
        Q_ERROR_ID(330);
    }

    if (HAL_TIMEx_MasterConfigSynchronization(tim_motor, &tim_mstr_cfg) != HAL_OK) {
        Q_ERROR_ID(340);
    }

    /* DRV IN1 */
    if (HAL_TIM_PWM_ConfigChannel(tim_motor, &pwm_tim_cfg, TIM_CHANNEL_1) != HAL_OK) {
        Q_ERROR_ID(350);
    }

    /* DRV IN2 */
    if (HAL_TIM_PWM_ConfigChannel(tim_motor, &pwm_tim_cfg, TIM_CHANNEL_2) != HAL_OK) {
        Q_ERROR_ID(360);
    }
}

/**
 * @brief BSP_InitTimers
 *          Initialise all the timers used by the decoder.
 */
void BSP_InitTimers(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_TIM21_CLK_ENABLE();
    __HAL_RCC_TIM22_CLK_ENABLE();

    DCC_TIM_Init();
    Tick_TIM_Init();
    MotorPWM_TIM_Init();
}

/**
 * @brief BSP_RegisterDCCTimIsr
 *          Registers the isr used to decode the DCC signal
 * @param isr (void (*)(void)):
 *          Function pointer to the desired isr
 */
void BSP_RegisterDCCTimIsr(void (*isr)(void))
{
    dcc_tim_isr = isr;
}

void BSP_StartDCCTimer(void)
{
    __HAL_TIM_SET_COUNTER(tim_dcc, 0u);

    // __HAL_TIM_ENABLE_IT(tim_dcc, TIM_IT_UPDATE);

    __HAL_TIM_ENABLE(tim_dcc);
}

void BSP_StopDCCTimer(void)
{
    __HAL_TIM_DISABLE_IT(tim_dcc, TIM_IT_UPDATE);

    __HAL_TIM_DISABLE(tim_dcc);
}

/**
 * @brief BSP_GetDCCTimer
 *          Get the actual value of the counter of the DCC decoding timer
 * @return counter (uint32_t):
 *          Value of the CNT register of the DCC timer
 */
inline uint32_t BSP_GetDCCTimer(bool * const ovf)
{
    Q_ENSURE(ovf != NULL);

    if ((tim_dcc->Instance->SR & TIM_SR_UIF) != 0) {
        tim_dcc->Instance->SR = 0u;
        *ovf = true;
    }
    else {
        *ovf = false;
    }

    return __HAL_TIM_GET_COUNTER(tim_dcc);
}

void BSP_SetMotorPwm1(uint16_t dc)
{
    __HAL_TIM_SET_COMPARE(tim_motor, TIM_CHANNEL_1, dc);
}

void BSP_SetMotorPwm2(uint16_t dc)
{
    __HAL_TIM_SET_COMPARE(tim_motor, TIM_CHANNEL_2, dc);
}

/**
 * @brief TIM2_IRQHandler
 *          TIM2 global interrupt ISR
 *
 *         NOTE: Function in not used (IRQ not activated)
 */
void TIM2_IRQHandler(void)
{
    /* No other interrupt aside TIM_FLAG_UPDATE should be generated */
    /* TODO: Investigate why there are also interrupt flags of CCxIF and the
     * DMAR is changed after the first IRQ */
    //Q_ENSURE(tim_dcc->Instance->SR == TIM_FLAG_UPDATE);

    /* Clear UE flag */
    __HAL_TIM_CLEAR_FLAG(tim_dcc, TIM_IT_UPDATE);

    /* Call the registered ISR */
    if (NULL != dcc_tim_isr) {
        dcc_tim_isr();
    }
}

/**
 * @brief TIM21_IRQHandler
 *          TIM21 global interrupt ISR
 */
void TIM21_IRQHandler(void)
{
    /* No other interrupt aside TIM_FLAG_UPDATE should be generated */
    Q_ENSURE(tim_tick->Instance->SR == TIM_FLAG_UPDATE);

    /* Clear UE flag */
    __HAL_TIM_CLEAR_FLAG(tim_tick, TIM_IT_UPDATE);
}

