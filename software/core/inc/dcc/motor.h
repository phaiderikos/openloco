/*******************************************************************************
 * @file    :   motor.h
 * @brief   :   Functions related to controlling the motor
 * @author  :   Davide Campagna
 * @date    :   Oct 13, 2022
 * @version :   V1.0
*******************************************************************************/

#ifndef __DCC_MOTOR_H
#define __DCC_MOTOR_H

#include "qm_motor.h"

uint32_t configure_motor(motor * const me, QEvt const * const e);

uint32_t motor_reset_substeps(motor * const me);

uint32_t compute_substep_param(motor * const me);

bool is_stopped(motor const * const me);

bool is_at_speed(motor const * const me);

void brake(bool fast);

void motor_pwm_set(int16_t value);

void motor_dir_set(enum motor_direction dir);

void emergency_brake(void);

#endif		/* __DCC_MOTOR_H */
