/*******************************************************************************
 * @file    :   dcc_funct.c
 * @brief   :   
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0
*******************************************************************************/

#include "dcc_funct.h"
#include "cv.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>

/**
 * Decoder Control (0000)
 */
uint8_t dcc_dec_ctrl(uint8_t instr, uint8_t data, uint8_t data_c)
{
	uint8_t sub_i;
	uint8_t tmp;

	sub_i = instr & 0x0fu;
	switch (sub_i) {
		case DCC_DC_RST:
			if (data_c < 2) {
				return DCC_ERROR;
			}

			if (data == 1) {
				/* Hard Reset */
				write_cv(19, 0x00);
				write_cv(29, CV29);
				write_cv(31, CV31);
				write_cv(32, CV32);
				/* than call Digital Decoder Reset */
			} else if (data == 0) {
				/* Digital Decoder Reset */
				// TODO: implement
			}
			break;
		case DCC_DC_FTI:
			/* Factory Test */
			// TODO: implement
			break;
		case DCC_DC_RES1:
		case DCC_DC_RES2:
		case DCC_DC_RES3:
			/* Reserved for future use */
			break;
		case DCC_DC_SDF:
			/* Set Decoder Flag */
			// TODO: implement
			break;
		case DCC_DC_SAA:
			/* Decoder Acknowledgment Request */
			tmp = read_cv(29);
			write_cv(29, tmp | 0x20);
			// TODO: implement
			break;
		case DCC_DC_DAR:
			/* Decoder Acknowledgment Request */
			// TODO: implement
			break;
		default:
			break;
	}

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_cons_ctrl(uint8_t instr, uint8_t data, uint8_t data_c)
{
	uint8_t sub_i;

	if (data_c < 2) {
		return DCC_ERROR;
	}

	sub_i = instr & 0x0fu;

	switch (sub_i) {
		case DCC_CC_FWD:
			/* Consist direction: Normal */
			break;
		case DCC_CC_BWD:
			/* Consist direction: Opposite */
			break;
		default:
			return DCC_ERROR;
	}

	if (data == 0)
		;
		/* Consist deactivated */
	else
		/* Consist activated with address */
		;

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_cv_acc_s(uint8_t instr, const uint8_t *buffer, uint8_t data_c)
{
	uint8_t cv;

	if (data_c < 2) {
		return DCC_ERROR;
	}

	cv = instr & 0x0fu;

	switch (cv) {
		case 0x0:
			/* Address 0x0 not available for use */
			return DCC_ERROR;
		case 0x2:
			/* Acceleration value CV#23 */
			break;
		case 0x3:
			/* Deceleration value CV#24: %d */
			break;
		case 0x9:
			/* S-9.2.3, Appendix B */
			break;
		default:
			/* Address is reserved */
			return DCC_ERROR;
	}

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_cv_acc_l(uint8_t instr, const uint8_t *buffer, uint8_t data_c)
{
	uint8_t  i_type;
	uint16_t cv;

	if (data_c < 3) {
		return DCC_ERROR;
	}

	cv = ((instr & 0x3u) << 8u | *buffer++) + 1;

	i_type = (instr & 0x0Cu) >> 2u;
	switch (i_type) {
		case 0x0:
			/* Reserved for future use */
			break;
		case 0x1:
			/* Verify byte CV# */
			break;
		case 0x3:
			/* Write byte CV# */
			break;
		case 0x2:
			/* Bit manipulation */
			if (*buffer & 0x10u) {
				/* CV write bit */
				// bit = *buffer & 0x07u;
				// val = *buffer & 0x08u;
			} else {
				/* CV verify bit */
				// bit = *buffer & 0x07u;
				// val = *buffer & 0x08u;
			}
			break;
		default:
			break;
	}

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_ana_fun_g(const uint8_t *buffer, uint8_t data_c)
{
	/* Analog Function Group */

	if (data_c < 3) {
		return DCC_ERROR;
	}

	uint8_t a_out  = *buffer++;
	uint8_t a_data = *buffer;

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_128_speed(const uint8_t *buffer, uint8_t data_c)
{
	uint8_t dir, speed;

	/* 128 Speed Step Control */

	if (data_c < 2) {
		return DCC_ERROR;
	}

	dir   = (*buffer & 0x80u) >> 7u;
	speed = *buffer & 0x7fu;

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_fun_21_28(const uint8_t *buffer, uint8_t data_c)
{
	uint8_t fun;

	/* F21-F28 Function Control */

	if (data_c < 2) {
		return DCC_ERROR;
	}

	fun = *buffer & 0x7fu;

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_fun_13_20(const uint8_t *buffer, uint8_t data_c)
{
	uint8_t fun;

	/* F13-F20 Function Control */

	if (data_c < 2) {
		return 1;
	}

	fun = *buffer & 0x7fu;

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_bin_state_s(const uint8_t *buffer, uint8_t data_c)
{
	uint8_t state, addr;

	/* Binary State Control Instruction short form */
	if (data_c < 2) {
		return 1;
	}

	state = *buffer >> 7u;
	addr  = *buffer & 0x7fu;
	if (addr == 0) {
		/* Set/Reset all binary states */
	} else {
		/* Set/Reset binary state */
	}

	return DCC_OK;
}

// TODO: implement
uint8_t dcc_bin_state_l(const uint8_t *buffer, uint8_t data_c)
{
	uint8_t  state;
	uint16_t addr;

	/* Binary State Control Instruction long form */
	if (data_c < 3) {
		return DCC_ERROR;
	}

	state = *buffer >> 7u;
	addr  = *buffer++ & 0x7fu;
	addr |= (uint16_t) (*buffer << 7u);
	if (addr == 0) {
		/* Set/Reset all binary states */
	} else {
		/* Set/Reset binary state */
	}

	return DCC_OK;
}

// TODO: implement
void dcc_fun_g1(uint8_t instr)
{
	uint8_t fun;

	/* Function Group 1 Instruction */

	fun = instr & 0x1fu;

	/* FL,F4-F1 */
}

// TODO: implement
void dcc_fun_g2(uint8_t instr)
{
	uint8_t fun;

	/* Function Group 2 Instruction */

	fun = instr & 0x1fu;

	if (fun & 0x10u) {
		/* F12 - F9 */
	} else {
 		/* F8 - F5 */
	}
}

// TODO: implement
/**
 * If Bit 1 of CV#29 is set, bit 4 is used as an intermediate speed step; else
 * it is used to control FL (front headlight)
 */
void dcc_vel_dir(uint8_t instr)
{
	uint8_t dir, speed;

	/* Speed and Direction Instruction */
	dir   = instr & 0x20u;
	speed = instr & 0x1fu;

	if ((speed & 0x0f) == 0x00) {
		/* Stop */
	} else if ((speed & 0x0f) == 0x01) {
		/* Emergency stop! */
	} else {

	}
}

// TODO: implement
uint8_t dcc_clamp_speed(const uint8_t *buffer, uint8_t data_c)
{
	uint8_t speed;

	/* Restricted Speed Step Instruction */
	if (data_c < 2) {
		return 1;
	}

	uint8_t mode = (*buffer & 0x80u) >> 7u;
	speed = *buffer & 0x1fu;

	return DCC_OK;
}
