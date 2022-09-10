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
#include "main.h"

#include <stdlib.h>

const uint32_t ONE_MIN = 52;		/* 52 µs */
const uint32_t ONE_MAX = 64;		/* 64 µs */
const uint32_t ONE_DELTA = 6;		/* 6  µs */

const uint32_t ZERO_MIN = 90;		/* 90 µs */
const uint32_t ZERO_MAX = 10000;	/* 10000 µs  */
const uint32_t ZERO_COMPL = 12000;	/* 12000 µs  */

struct decoder dec1;

void interrupt_funct(uint16_t T)
{
	if (ONE_MIN < T && T < ONE_MAX) { /* pulse duration is within ONE timings */
		if (dec1.half1 == true)	{
			/* it's second part of a 1-bit */

			if (abs(T - dec1.T_prev) <= ONE_DELTA) {
				/* difference between first and second part is within limits */

				if (dec1.has_preamble == true) {
					/* already have a preamble, it's a bit */

					if (dec1.N == 8) {
						/* end of packet bit */
						dec1.bytes[dec1.byte_n] = dec1.actual_byte;
						dec1.byte_n++;

						decoder_end(&dec1);
						return;
					} else {
						/* normal data bit */
						uint8_t mask = 1 << (7 - dec1.N);

						dec1.actual_byte |= mask;
						dec1.N++;
						dec1.half1 = false;
						return;
					}
				} else {
					/* has no preamble yet, it's a preamble bit */
					dec1.N++;
					dec1.half1 = false;
					return;
				}
			} else {
				/* difference between two parts outside limits */
				decoder_reset(&dec1);
				return;
			}
		}
		else	/* first part of the bit */
		{
			dec1.half1 = true;
			dec1.half0 = false;
			dec1.T_prev = T;
			return;
		}
	} else if (ZERO_MIN < T && T < ZERO_MAX) {
		/* pulse duration is within ZERO timings */

		if (dec1.half0 == true) {
			/* it's second part of a 0-bit */

			if (T + dec1.T_prev > ZERO_COMPL) {
				/* Total bit length outside limits*/
				decoder_reset(&dec1);
				return;
			} else {
				if (dec1.has_preamble) {
					/* already have a preamble, it's a bit */

					if (dec1.N == 8) {
						/* bit that marks the end of the byte */
						dec1.bytes[dec1.byte_n] = dec1.actual_byte;
						dec1.byte_n++;
						dec1.N = 0;

						dec1.half0 = false;
						dec1.actual_byte = 0;
						return;
					} else	{
						/* normal data bit */
						dec1.N++;
						dec1.half0 = false;
						return;
					}
				} else {
					/* No preamble yet, it's the end of a potential preamble */

					if (dec1.N >= 10) {
						/* Valid preamble */
						dec1.has_preamble = true;
						dec1.half0 = false;
						dec1.N = 0;
						return;
					} else {
						/* Invalid preamble */
						decoder_reset(&dec1);
						return;
					}
				}
			}
		} else {
			/* first part of the bit */
			dec1.half0 = true;
			dec1.half1 = false;
			dec1.T_prev = T;
			return;
		}
	} else {
		/* pulse duration marks neither a zero or a one */
		decoder_reset(&dec1);
	}

	return;
}

void decoder_reset(struct decoder *dec)
{
	dec->half1 = false;
	dec->half0 = false;
	dec->has_preamble = false;
	dec->T_prev = 0;
	dec->N = 0;
	dec->byte_n = 0;
}

void decoder_end(struct decoder *dec)
{
	decode(dec->bytes, dec->byte_n, 1);

	decoder_reset(dec);
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

	/* data bytes = total bytes - 2 bytes of address and error detection)
	*/
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
