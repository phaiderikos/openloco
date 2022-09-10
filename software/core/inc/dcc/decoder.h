/*******************************************************************************
 * @file    :   decoder.h
 * @brief   :   DCC Decoder layer 1
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0
*******************************************************************************/

#ifndef __DCC_DECODER_H
#define __DCC_DECODER_H

#include <stdint.h>
#include <stdbool.h>

#define DCC_DCCI        0x00     // Decoder and Consist Control Instruction
#define DCC_AOI         0x20     // Advanced Operation Instructions
#define DCC_SDIR        0x40     // Speed and Direction Instruction for reverse operation
#define DCC_SDIF        0x60     // Speed and Direction Instruction for forward operation
#define DCC_FG1I        0x80     // Function Group One Instruction
#define DCC_FG2I        0xA0     // Function Group Two Instruction
#define DCC_FE          0xC0     // Feature Expansion
#define DCC_CVAI        0xE0     // Configuration Variable Access Instruction


struct decoder
{
	uint8_t bytes[16];
	uint8_t N, byte_n;
	uint8_t actual_byte;
	uint16_t T_prev;
	bool half0, half1, has_preamble;
};

void decoder_reset(struct decoder *dec);

void decoder_end(struct decoder *dec);

uint8_t decode(const uint8_t *buffer, uint8_t len, uint8_t check);

void interrupt_funct(uint16_t T);

#endif //__DCC_DECODER_H
