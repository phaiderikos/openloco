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

#include "qm_decoder.h"

#define DCC_DCCI        0x00     // Decoder and Consist Control Instruction
#define DCC_AOI         0x20     // Advanced Operation Instructions
#define DCC_SDIR        0x40     // Speed and Direction Instruction for reverse operation
#define DCC_SDIF        0x60     // Speed and Direction Instruction for forward operation
#define DCC_FG1I        0x80     // Function Group One Instruction
#define DCC_FG2I        0xA0     // Function Group Two Instruction
#define DCC_FE          0xC0     // Feature Expansion
#define DCC_CVAI        0xE0     // Configuration Variable Access Instruction

void decoder_aux_dict(void);

void decoder_reset(void);

uint8_t decode(const uint8_t *buffer, uint8_t len, uint8_t check);

void post_dcc_message(decoder const * const me);

#endif //__DCC_DECODER_H
