#ifndef __QP_SIGNALS_H__
#define __QP_SIGNALS_H__

#include "stdint.h"
#include "stddef.h"
#include "qpc.h"

#define NO_ERROR	((uint32_t) 0u)

#define DCC_MAX_PACKET_LEN	(16u)

/* QEP custom signals */

enum {
	UNUSED_SIG = Q_USER_SIG,
	CNTRL_RECONFIGURE_SIG,
	DCC_MESSAGE_SIG,
	MOTOR_CONFIG_SIG,
	MOTOR_CTRL_START_SIG,
	MOTOR_CTRL_RECONFIGURE_SIG,
	EMERGENCY_BRAKE_SIG,
	EMERGENCY_BRAKE_RELEASE_SIG,
	CHANGE_SPEED_SIG,
	SPEED_REACHED_SIG,
	MOTOR_STOPPED_SIG,
	SET_SPEED_SIG,
	SPEED_TIMER_SIG,
	DECODER_START_SIG,
	DECODER_STOP_SIG,
	DECODER_RST_SIG,
	BIT_0_SIG,
	BIT_1_SIG,
};

/* Opaque AO pointers */
extern QActive * const ao_controller;
extern QActive * const ao_decoder;
extern QActive * const ao_motor;

#endif	/* __QP_SIGNALS_H__ */
