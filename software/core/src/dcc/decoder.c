/*******************************************************************************
 * @file    :   decoder.c
 * @brief   :   DCC Decoder layer 1
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0
*******************************************************************************/

#include "decoder.h"
#include "dcc_funct.h"
#include "config.h"
#include "qp_signals.h"
#include "qm_controller.h"

#include "bsp.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

Q_DEFINE_THIS_MODULE("Decoder");

/* ----- DCC bit times definitions ---- */
#define ONE_MIN		((uint32_t) 52*4u)	/* 52 µs 	*/
#define ONE_MAX		((uint32_t) 64*4u)	/* 64 µs 	*/
#define ONE_DELTA	((uint32_t) 6u*4u)	/* 6  µs 	*/

#define ZERO_MIN	((uint32_t) 90u*4u)	/* 90 µs 	*/
#define ZERO_MAX	((uint32_t) 10000u*4u)	/* 10000 µs	*/
#define ZERO_COMPL	((uint32_t) 12000u*4u)	/* 12000 µs	*/

/* static functions prototypes */
static void decoder_edge_isr(void);
static void decoder_tim_isr(void);

static void interrupt_funct(uint16_t const pulse);

static volatile bool dcc_timer_elapsed = false;

#ifdef Q_SPY
/* Local QSpy source IDs */
static QSpyId const decoderISR = { 0U };
#endif

static void decoder_edge_isr(void)
{
	static uint16_t last_value = 0u;

	uint16_t value;
	bool ovf;

	value = BSP_GetDCCTimer(&ovf);

	uint16_t new_val = value;

	if (value > last_value) {
		value -= last_value;
		if (ovf) {
			/* Unexpected timer overflow */
			value = 65535u;
		}
	}
	else {
		/* Just 1 timer overflow */
		value = (65535u - last_value) + value;
	}

	last_value = new_val;

	interrupt_funct(value);
}

static void decoder_tim_isr(void)
{
	/* NOTE: unused right now */
	// dcc_timer_elapsed = true;
}

static void interrupt_funct(uint16_t const pulse)
{
#if 0
	static uint16_t timings[64] = {0u};
	static uint32_t index = 0u;
#endif
	static bool half1 = false;
	static bool half0 = false;
	static uint16_t T_prev = 0u;

	static uint16_t min_pulse = 0xffffu;
	static uint16_t max_pulse = 0u;

	if (pulse < min_pulse) {
		min_pulse = pulse;
	} else if (pulse > max_pulse) {
		max_pulse = pulse;
	}

#if 0
	timings[index++] = pulse;
	if (index >= sizeof(timings)) {
		index = 0u;
	}
#endif

	if (ONE_MIN < pulse && pulse < ONE_MAX) {
		/* pulse duration is within ONE timings */
		if (half1) {
			/* it's second part of a 1-bit */
			if (pulse <= T_prev + ONE_DELTA && pulse >= T_prev - ONE_DELTA) {
				/* difference between first and second part is within limits: it's a valid 1 */
				/* self post a BIT_1 event */
				static QEvt const bit1 = QEVT_INITIALIZER(BIT_1_SIG);

				QACTIVE_POST(ao_decoder, &bit1, &decoderISR);
			}
			half1 = false;
		} else {
			/* first part of the bit */
			half1 = true;
			half0 = false;
			T_prev = pulse;
		}
	} else if ((ZERO_MIN < pulse) && (pulse < ZERO_MAX)) {
		/* pulse duration is within ZERO timings */

		if (half0) {
			/* it's second part of a 0-bit */

			if ((pulse + T_prev) > ZERO_COMPL) {
				/* Total bit length outside limits*/
				half0 = false;
			} else {
				/* self post a BIT_0 event */
				static QEvt const bit0 = QEVT_INITIALIZER(BIT_0_SIG);

				QACTIVE_POST(ao_decoder, &bit0, &decoderISR);
			}
		} else {
			/* first part of the bit */
			half0 = true;
			half1 = false;
			T_prev = pulse;
			return;
		}
	} else {
		/* pulse duration marks neither a zero nor a one */
		half0 = true;
		half1 = false;
		/* self post a decoder reset evt */
		static QEvt const rst = QEVT_INITIALIZER(DECODER_RST_SIG);


		QACTIVE_POST(ao_decoder, &rst, &decoderISR);
	}
}

void decoder_aux_dict(void)
{
	QS_OBJ_DICTIONARY(&decoderISR);
}

void decoder_reset(void)
{
	//dec->half1 = false;
	//dec->half0 = false;
	//dec->has_preamble = false;
	//dec->T_prev = 0;
	//dec->N = 0;
	//dec->byte_n = 0;
	BSP_RegisterDCCExtIIsr(decoder_edge_isr);
	BSP_RegisterDCCTimIsr(decoder_tim_isr);
}

uint8_t decode(const uint8_t *buffer, uint8_t len, uint8_t check)
{
	unsigned char parse = 0;

	if (len < 3) {
		return DCC_ERROR;
	}

	uint8_t sum = 0x00;

	if (check) {
		for (uint8_t i = 0; i < len; i++)
			sum ^= buffer[i];

		if (sum != 0)
			return DCC_ERROR;
	}

	/* data bytes = total bytes - 2 bytes of address and error detection) */
	uint8_t data_c = len - 2;

	if (data_c == 0) {
		return DCC_ERROR;
	}

	uint16_t address = 0;

	if (*buffer == DCC_BROADCAST) {
		parse = 1;
		buffer++;
	} else if (*buffer == DCC_IDLEADDR) {
		return DCC_IDLE;
	} else {
		/**
		 * If address starts with 11, a second address byte must follow
		 */
		if (*buffer & 0xc0u) { // 2-byte address
			address = (*buffer++ & 0x3fu) << 8u;
			address |= *buffer++;
			data_c--;
			if (data_c == 0) {
				return DCC_ERROR;
			}
		} else {        // 1-byte address
			address |= *buffer++;
		}

		if (address == DCC_ADDRESS) {
			parse = 1;
		}
	}

	if (parse) {
		uint8_t instr  = *buffer++;
		uint8_t i_type = instr & 0xe0u;
		uint8_t sub_i;

		switch (i_type) {
			case DCC_DCCI:
				if (instr & 0x10u) { // 0001 - Consist Control
					if (dcc_cons_ctrl(instr, *buffer, data_c)) {
						return DCC_ERROR;
					}
				} else {        // 0000 - Decoder Control
					if (dcc_dec_ctrl(instr, *buffer, data_c)) {
						return DCC_ERROR;
					}
				}
				break;
			case DCC_AOI:
				/* Advanced Operations Instruction */
				sub_i = instr & 0x1fu;
				switch (sub_i) {
					case 0x1f:
						if (dcc_128_speed(buffer, data_c)) {
							return DCC_ERROR;
						}
						break;
					case 0x1e:
						if (dcc_clamp_speed(buffer, data_c)) {
							return DCC_ERROR;
						}
						break;
					case 0x1d:
						if (dcc_ana_fun_g(buffer, data_c)) {
							return DCC_ERROR;
						}
						break;
					default:
						/* Reserved for future use */
						break;
				}
				break;
			case DCC_SDIR:
			case DCC_SDIF:
				dcc_vel_dir(instr);
				break;
			case DCC_FG1I:
				dcc_fun_g1(instr);
				break;
			case DCC_FG2I:
				dcc_fun_g2(instr);
				break;
			case DCC_FE:
				/* Feature Expansion Instruction */
				sub_i = instr & 0x1fu;
				switch (sub_i) {
					case DCC_FE_BSCI_L:
						if (dcc_bin_state_l(buffer, data_c)) {
							return DCC_ERROR;
						}
						break;
					case DCC_FE_BSCI_S:
						if (dcc_bin_state_s(buffer, data_c)) {
							return DCC_ERROR;
						}
						break;
					case DCC_FE_F1320:
						if (dcc_fun_13_20(buffer, data_c)) {
							return DCC_ERROR;
						}
						break;
					case DCC_FE_F2128:
						if (dcc_fun_21_28(buffer, data_c)) {
							return DCC_ERROR;
						}
						break;
					default:
						break;
				}
				break;
			case DCC_CVAI:
				/* Configuration Variable Access Instruction */
				if (instr & 0x10u) {
					dcc_cv_acc_s(instr, buffer, data_c);
				} else {
					dcc_cv_acc_l(instr, buffer, data_c);
				}
				break;
			default:
				break;
		}
	} else {
		return DCC_IGNORE;
	}

	return DCC_OK;
}

void post_dcc_message(decoder const * const me)
{
	Q_REQUIRE(NULL != me);

	dcc_message * const dcc_msg = Q_NEW(dcc_message, DCC_MESSAGE_SIG);

	/* Set packet size */
	dcc_msg->size = me->byte_cnt;

	/* Copy packet data to the message */
	memcpy(dcc_msg->data, me->rx_buffer, me->byte_cnt);

	/* Post the event */
	QACTIVE_POST(ao_controller, &dcc_msg->super, ao_decoder);
}
