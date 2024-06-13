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

#define SMALL_POOL_SIZE		(64u)
#define MEDIUM_POOL_SIZE	(32u)
#define LARGE_POOL_SIZE		(16u)

static QEvt const * motor_queue[32u];
static QEvt const * controller_queue[32u];
static QEvt const * decoder_queue[32u];

static speed_evt small_pool_sto[SMALL_POOL_SIZE];
static motor_cfg_evt large_pool_sto[LARGE_POOL_SIZE];

/**
 * @brief  The application entry point.
 *
 * @retval unused (int):
 * 		main function never returns
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

	QF_init();	/* Initialise the framework and the underlying RT kernel */

	QF_poolInit(small_pool_sto, sizeof(small_pool_sto), sizeof(small_pool_sto[0]));
	QF_poolInit(large_pool_sto, sizeof(large_pool_sto), sizeof(large_pool_sto[0]));

	/* Call the constructor of the active objects */
	decoder_ctor();

	motor_ctor();

	controller_ctor();

	/* Start the active objects */
	QACTIVE_START(	ao_motor,					/* Pointer to Motor AO to start				*/
			QF_MOTOR_PRIO,					/* AO priority						*/
			motor_queue, Q_DIM(motor_queue),		/* Storage and size of event queue for Motor AO		*/
			(void *)0u, 0u,					/* No stack space is used in QV				*/
			(QEvt *)0u);					/* No initial event					*/

	QACTIVE_START(	ao_decoder,					/* Pointer to Decoder AO to start			*/
			QF_DECODER_PRIO,				/* AO priority						*/
			decoder_queue, Q_DIM(decoder_queue),		/* Storage and size of event queue for Controller AO	*/
			(void *)0u, 0u,					/* No stack space is used in QV				*/
			(QEvt *)0u);					/* No initial event					*/

	QACTIVE_START(	ao_controller,					/* Pointer to Controller AO to start 			*/
			QF_CONTROLLER_PRIO,				/* AO priority 						*/
			controller_queue, Q_DIM(controller_queue),	/* Storage and size of event queue for Controller AO 	*/
			(void *)0u, 0u,					/* No stack space is used in QV 			*/
			(QEvt *)0u);					/* No initial event 					*/

	for (;;) {
		QF_run();
	}
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file(uint8_t *):
 * 		pointer to the source file name
 * @param  line(uint32_t):
 * 		error line
 */
void assert_failed(uint8_t * file, uint32_t line)
{
	Q_onError((char *)file, line);
}
#endif	/* USE_FULL_ASSERT */
