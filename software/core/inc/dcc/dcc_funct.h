/*******************************************************************************
 * @file    :   dcc_funct.h
 * @brief   :   
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0
*******************************************************************************/

#ifndef DCC_ENCODER_MAIN_H
#define DCC_ENCODER_MAIN_H

#include <stdint.h>

#define DCC_DC_RST      0x0
#define DCC_DC_FTI      0x2
#define DCC_DC_RES1      0x4
#define DCC_DC_SDF      0x6
#define DCC_DC_RES2      0x8
#define DCC_DC_SAA       0xa
#define DCC_DC_RES3       0xc
#define DCC_DC_DAR      0xe

#define DCC_CC_FWD      0x02
#define DCC_CC_BWD      0x03

#define DCC_FE_BSCI_L   0x00
#define DCC_FE_BSCI_S   0x1d
#define DCC_FE_F1320    0x1e
#define DCC_FE_F2128    0x1f

uint8_t decode(const uint8_t *buffer, uint8_t len, uint8_t check);

uint8_t dcc_dec_ctrl(uint8_t instr, uint8_t data, uint8_t data_c);

uint8_t dcc_cons_ctrl(uint8_t instr, uint8_t data, uint8_t data_c);

/**
 * @brief Speed and Direction Instructions
 * @description A speed and direction instruction is used send information to
 * motors connected to Multi Function Digital Decoders. Instruction "010"
 * indicates a Speed and Direction Instruction for reverse operation and
 * instruction "011" indicates a Speed and Direction Instruction for forward
 * operation. In these instructions the data is used to control speed with bits
 * 0-3 being defined exactly as in S-9.2 Section B. If Bit 1 of CV#29 has a
 * value of one (1), then bit 4 is used as an intermediate speed step, as
 * defined in S-9.2, Section B. If Bit 1 of CV#29 has a value of zero (0), then
 * bit 4 shall be used to control FL3. In this mode, Speed U0000 is stop, speed
 * U0001 is emergency stop, speed U0010 is the first speed step and speed U1111
 * is full speed. This provides 14 discrete speed steps in each direction. If a
 * decoder receives a new speed step that is within one step of current speed
 * step, the Digital Decoder may select a step half way between these two speed
 * steps. This provides the potential to control 56 speed steps should the
 * command station alternate speed packets. Decoders may ignore the direction
 * information transmitted in a broadcast packet for Speed and Direction
 * commands that do not contain stop or emergency stop information.
 */
void dcc_vel_dir(uint8_t instr);

/**
 * @brief Function Group One Instruction (100)
 * Up to 5 auxiliary functions (functions FL and F1-F4) can be controlled by the
 * Function Group One instruction.
 */
void dcc_fun_g1(uint8_t instr);

/**
 * @brief Function Group One Instruction (101)
 * Up to 8 additional auxiliary functions (F5-F12) can be controlled by a
 * Function Group Two instruction. Bit 4 defines the use of Bits 0-3.
 */
void dcc_fun_g2(uint8_t instr);

/**
 * @brief Binary State Control Instruction long form
 * Sub instruction "00000" is a three byte instruction and provides for control
 * of one of 32767 binary states within the decoder.
 */
uint8_t dcc_bin_state_l(const uint8_t *buffer, uint8_t data_c);

/**
 * @brief Binary State Control Instruction short form
 * Sub-instruction “11101” is a two byte instruction and provides for control of
 * one of 127 binary states within the decoder
 */
uint8_t dcc_bin_state_s(const uint8_t *buffer, uint8_t data_c);

/**
 * @brief F13-F20 Function Control
 * Sub-instruction “11110” is a two byte instruction and provides for control of
 * eight (8) additional auxiliary functions F13-F20.
 */
uint8_t dcc_fun_13_20(const uint8_t *buffer, uint8_t data_c);

/**
 * @brief F21-F28 Function Control
 * Sub-instruction “11111” is a two byte instruction and provides for control of
 * eight (8) additional auxiliary functions F21-F28.
 */
uint8_t dcc_fun_21_28(const uint8_t *buffer, uint8_t data_c);

/**
 * @brief 128 Speed Step Control
 * @description Instruction "11111" is used to send one of 126 Digital Decoder 
 * speed steps. The subsequent single byte shall define speed and direction with
 * bit 7 being direction ("1" is forward and "0" is reverse) and the remaining
 * bits used to indicate speed. The most significant speed bit is bit 6.
 * A data-byte value of U0000000 is used for stop, and a data-byte value of
 * U0000001 is used for emergency stop. This allows up to 126 speed steps. When
 * operations mode acknowledgment is enabled, receipt of a 128 Speed Step
 * Control packet must be acknowledged with an operations mode acknowledgement.
 */
uint8_t dcc_128_speed(const uint8_t *buffer, uint8_t data_c);

/**
 * @brief Restricted Speed Step Instruction
 * @description Instruction “11110” is used to restrict the maximum speed of a
 * decoder. Bit 7 of the data byte (‘DDDDDDDD’ above) is used to enable (‘0’) or
 * disable (‘1’) restricted speed operation. Bits 0-5 of the Data byte are the
 * Speed Steps defined in S-9.22. When operations mode acknowledgment is
 * enabled, receipt of a Restricted Speed Instruction must be acknowledged with 
 * an operations mode acknowledgement.
 */
uint8_t dcc_clamp_speed(const uint8_t *buffer, uint8_t data_c);

/**
 * @brief Analog Function Group
 */
uint8_t dcc_ana_fun_g(const uint8_t *buffer, uint8_t data_c);

/**
 * @brief Configuration Variable Access Instruction - Short Form
 */
uint8_t dcc_cv_acc_s(uint8_t instr, const uint8_t *buffer, uint8_t data_c);

/**
 * @brief Configuration Variable Access Instruction - Long Form
 */
uint8_t dcc_cv_acc_l(uint8_t instr, const uint8_t *buffer, uint8_t data_c);


enum dec_res {
        DCC_OK, DCC_IDLE, DCC_IGNORE, DCC_ERROR
};

#endif //DCC_ENCODER_MAIN_H

/**
 * @brief
 */
