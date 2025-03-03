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
#include "main.h"
#include "bsp.h"

#include <stdlib.h>
#include <string.h>

#define INSTRUCTION_MASK    (0x0Fu)

#define STATE_SHIFT         (0x07u)
#define ADDRESS_MASK        (0x7Fu)

#define FNC1_MASK           (0x01u)
#define FNC2_MASK           (0x02u)
#define FNC3_MASK           (0x04u)
#define FNC4_MASK           (0x08u)

/**
 * Decoder Control (0000)
 */
uint8_t dcc_dec_ctrl(uint8_t instr, uint8_t data, uint8_t data_c)
{
    uint8_t sub_i;
    uint8_t tmp;

    sub_i = instr & (INSTRUCTION_MASK);
    switch (sub_i) {
        case DCC_DC_RST: {
            if (data_c < 2) {
                return DCC_ERROR;
            }

            if (data == 1) {
                /* Hard Reset */
                write_cv(19, 0x00);
                write_cv(29, CV29);
                /* TODO: than call Digital Decoder Reset */
            } else if (data == 0) {
                /* Digital Decoder Reset */
                /* TODO: implement */
            }
            break;
        }
        case DCC_DC_FTI: {
            /* Factory Test */
            /* TODO: implement */
            break;
        }
        case DCC_DC_RES1:
        case DCC_DC_RES2:
        case DCC_DC_RES3: {
            /* Reserved for future use */
            break;
        }
        case DCC_DC_SDF: {
            /* Set Decoder Flag */
            /* TODO: implement */
            break;
        }
        case DCC_DC_SAA: {
            /* Decoder Acknowledgment Request */
            tmp = read_cv(29u);
            write_cv(29u, tmp | 0x20u);
            /* TODO: implement */
            break;
        }
        case DCC_DC_DAR: {
            /* Decoder Acknowledgment Request */
            /* TODO: implement */
            break;
        }
        default: {
            break;
        }
    }

    return DCC_OK;
}

/* TODO: implement */
uint8_t dcc_cons_ctrl(uint8_t instr, uint8_t data, uint8_t data_c)
{
    uint8_t sub_i;

    if (data_c < 2) {
        return DCC_ERROR;
    }

    sub_i = instr & (INSTRUCTION_MASK);

    switch (sub_i) {
        case DCC_CC_FWD: {
            /* Consist direction: Normal */
            break;
        }
        case DCC_CC_BWD: {
            /* Consist direction: Opposite */
            break;
        }
        default: {
            return DCC_ERROR;
        }
    }

    if (data == 0) {
        /* Consist deactivated */
    } else {
        /* Consist activated with address */
    }

    return DCC_OK;
}

/* TODO: implement */
uint8_t dcc_cv_acc_s(uint8_t instr, const uint8_t * buffer, uint8_t data_c)
{
    uint8_t status = DCC_OK;
    uint8_t cv = 0u;

    if (data_c < 2) {
        status = DCC_ERROR;
    } else {
        cv = instr & (INSTRUCTION_MASK);

        switch (cv) {
            case 0x0u: {
                /* Address 0x0 not available for use */
                status = DCC_ERROR;
                break;
            }
            case 0x2u: {
                /* Acceleration value CV#23 */
                break;
            }
            case 0x3u: {
                /* Deceleration value CV#24: %d */
                break;
            }
            case 0x9u: {
                /* S-9.2.3, Appendix B */
                break;
            }
            default: {
                /* Address is reserved */
                status = DCC_ERROR;
                break;
            }
        }
    }

    return status;
}

/* TODO: implement */
uint8_t dcc_cv_acc_l(uint8_t instr, const uint8_t * buffer, uint8_t data_c)
{
    enum dec_res status = DCC_OK;
    uint8_t i_type = 0u;
    uint16_t cv = 0u;

    if (data_c < 3u) {
        status = DCC_ERROR;
    } else {
        cv = ((instr & 0x3u) << 8u | *buffer++) + 1;

        i_type = (instr & 0x0Cu) >> 2u;
        switch (i_type) {
            case 0x0: {
                /* Reserved for future use */
                break;
            }
            case 0x1: {
                /* Verify byte CV# */
                break;
            }
            case 0x3: {
                /* Write byte CV# */
                break;
            }
            case 0x2: {
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
            }
            default: {
                break;
            }
        }
    }

    return status;
}

/* TODO: implement */
uint8_t dcc_ana_fun_g(const uint8_t * buffer, uint8_t data_c)
{
    /* Analog Function Group */

    if (data_c < 3) {
        return DCC_ERROR;
    }

    uint8_t a_out = *buffer++;
    uint8_t a_data = *buffer;

    return DCC_OK;
}

/* TODO: implement */
uint8_t dcc_128_speed(const uint8_t * buffer, uint8_t data_c)
{
    uint8_t dir = 0u;
    uint8_t speed = 0u;

    /* 128 Speed Step Control */

    if (data_c < 2) {
        return DCC_ERROR;
    }

    dir = (*buffer & 0x80u) >> 7u;
    speed = *buffer & 0x7fu;

    return DCC_OK;
}

/* TODO: implement */
uint8_t dcc_fun_21_28(const uint8_t * buffer, uint8_t data_c)
{
    uint8_t fun = 0u;

    /* F21-F28 Function Control */

    if (data_c < 2) {
        return DCC_ERROR;
    }

    fun = *buffer & 0x7fu;

    return DCC_OK;
}

/* TODO: implement */
uint8_t dcc_fun_13_20(const uint8_t * buffer, uint8_t data_c)
{
    uint8_t fun = 0u;

    /* F13-F20 Function Control */

    if (data_c < 2) {
        return 1;
    }

    fun = *buffer & 0x7fu;

    return DCC_OK;
}

/* TODO: implement */
uint8_t dcc_bin_state_s(const uint8_t * buffer, uint8_t data_c)
{
    uint8_t state = 0u;
    uint8_t addr = 0u;

    /* Binary State Control Instruction short form */
    if (data_c < 2) {
        return 1;
    }

    state = *buffer >> STATE_SHIFT;
    addr = *buffer & 0x7fu;
    if (addr == 0) {
        /* Set/Reset all binary states */
    } else {
        /* Set/Reset binary state */
    }

    return DCC_OK;
}

/* TODO: implement */
uint8_t dcc_bin_state_l(const uint8_t * buffer, uint8_t data_c)
{
    uint8_t state = 0u;
    uint16_t addr = 0u;

    /* Binary State Control Instruction long form */
    if (data_c < 3) {
        return DCC_ERROR;
    }

    state = *buffer >> STATE_SHIFT;
    addr = *buffer++ & ADDRESS_MASK;
    addr |= (uint16_t) (*buffer << 7u);
    if (addr == 0u) {
        /* Set/Reset all binary states */
    } else {
        /* Set/Reset binary state */
    }

    return DCC_OK;
}

/* TODO: implement */
void dcc_fun_g1(uint8_t instr)
{
    uint8_t fun;

    /* Function Group 1 Instruction */

    fun = instr & 0x1fu;

    /* F1 */
    BSP_SetGPIOA_pin((fun & FNC1_MASK) != 0u ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* F2 */
    BSP_SetGPIOB_pin((fun & FNC2_MASK) != 0u ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* F3 */
    BSP_SetAUX1_pin((fun & FNC3_MASK) != 0u ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* F4 */
    BSP_SetAUX2_pin((fun & FNC4_MASK) != 0u ? GPIO_PIN_SET : GPIO_PIN_RESET);

    /* FL */
    /*if (read_cv(29) & 0x02) {
       if (fun & 0x10) {

       } else {

       }
       } */
}

/* TODO: implement */
void dcc_fun_g2(uint8_t instr)
{
    uint8_t fun = 0u;

    /* Function Group 2 Instruction */

    fun = instr & 0x1fu;

    if (fun & 0x10u) {
        /* F12 - F9 */
    } else {
        /* F8 - F5 */
    }
}

/* TODO: implement */
/**
 * If Bit 1 of CV#29 is set, bit 4 is used as an intermediate speed step; else
 * it is used to control FL (front headlight)
 */
void dcc_vel_dir(uint8_t instr)
{
    uint8_t dir = 0u;
    uint8_t speed = 0u;

    /* Speed and Direction Instruction */
    dir = instr & 0x20u;
    speed = instr & 0x1fu;

    if ((speed & 0x0f) == 0x00) {
        /* Stop */
    } else if ((speed & 0x0f) == 0x01) {
        /* Emergency stop! */
    } else {

    }
}

/* TODO: implement */
uint8_t dcc_clamp_speed(const uint8_t * buffer, uint8_t data_c)
{
    uint8_t speed = 0u;

    /* Restricted Speed Step Instruction */
    if (data_c < 2) {
        return 1;
    }

    uint8_t mode = (*buffer & 0x80u) >> 7u;
    speed = *buffer & 0x1fu;

    return DCC_OK;
}
