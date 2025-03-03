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

extern uint32_t configure_motor(motor * const me, QEvt const * const e);

extern uint32_t motor_reset_substeps(motor * const me);

extern uint32_t compute_substep_param(motor * const me);

extern bool is_stopped(motor const * const me);

extern bool is_at_speed(motor const * const me);

extern void brake(bool const fast);

extern void motor_pwm_set(int16_t const value, enum motor_direction const dir);

extern void emergency_brake(void);

#endif  /* __DCC_MOTOR_H */
