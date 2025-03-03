/*******************************************************************************
 * @file    :   main.c
 * @brief   :   Main application file
 * @author  :   Davide Campagna
 * @date    :   May 15, 2024
 * @version :   V1.0
 ******************************************************************************/

#include "main.h"
#include "bsp.h"

#include "decoder.h"
#include "qp_signals.h"

#include "qm_decoder.h"
#include "qm_controller.h"
#include "qm_motor.h"

Q_DEFINE_THIS_MODULE("main")

#define SMALL_POOL_SIZE     (64u)
#define MEDIUM_POOL_SIZE    (32u)
#define LARGE_POOL_SIZE     (16u)

#define AO_CONTROLLER_QUEUE_SIZE    (32u)
#define AO_MOTOR_QUEUE_SIZE         (32u)
#define AO_DECODER_QUEUE_SIZE       (32u)

static QEvt const * motor_queue[AO_MOTOR_QUEUE_SIZE];
static QEvt const * controller_queue[AO_CONTROLLER_QUEUE_SIZE];
static QEvt const * decoder_queue[AO_DECODER_QUEUE_SIZE];

static speed_evt small_pool_sto[SMALL_POOL_SIZE];
static motor_cfg_evt large_pool_sto[LARGE_POOL_SIZE];

/**
 * @brief The application entry point.
 *
 * @retval unused (int):
 *          main function never returns
 */
int main(void)
{
    /* Active Objects priorities ---------------------------------------- */
    enum {
        /* Lower priority */
        QF_MOTOR_PRIO = 1u,
        QF_CONTROLLER_PRIO,
        QF_DECODER_PRIO
        /* Higher priority */
    };

    /* MCU Configuration ------------------------------------------------ */
    BSP_Init();

    /* QF INITIALIZATION */
    /* Initialise the framework and the underlying RT kernel */
    QF_init();

    QF_poolInit(small_pool_sto, sizeof(small_pool_sto), sizeof(small_pool_sto[0]));
    QF_poolInit(large_pool_sto, sizeof(large_pool_sto), sizeof(large_pool_sto[0]));

    /* Call the constructor of the active objects */
    decoder_ctor();

    motor_ctor();

    controller_ctor();

    /* Start the active objects */
    QACTIVE_START(
        /* Pointer to Motor AO to start */
        ao_motor,
        /* AO priority */
        QF_MOTOR_PRIO,
        /* Storage and size of event queue for Motor AO */
        motor_queue, Q_DIM(motor_queue),
        /* No stack space is used in QV */
        (void *)0u, 0u,
        /* No initial event */
        (QEvt *)0u
    );

    QACTIVE_START(
        /* Pointer to Decoder AO to start */
        ao_decoder,
        /* AO priority */
        QF_DECODER_PRIO,
        /* Storage and size of event queue for Controller AO */
        decoder_queue, Q_DIM(decoder_queue),
        /* No stack space is used in QV */
        (void *)0u, 0u,
        /* No initial event */
        (QEvt *)0u
    );

    QACTIVE_START(
        /* Pointer to Controller AO to start */
        ao_controller,
        /* AO priority */
        QF_CONTROLLER_PRIO,
        /* Storage and size of event queue for Controller AO */
        controller_queue, Q_DIM(controller_queue),
        /* No stack space is used in QV */
        (void *)0u, 0u,
        /* No initial event */
        (QEvt *)0u
    );

    for (;;) {
        QF_run();
    }
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief Reports the name of the source file and the source line number
 *          where the assert_param error has occurred.
 * @param file(uint8_t *):
 *          pointer to the source file name
 * @param line(uint32_t):
 *          error line
 */
void assert_failed(uint8_t * file, uint32_t line)
{
    Q_onError((char *)file, line);
}
#endif    /* USE_FULL_ASSERT */
