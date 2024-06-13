/*******************************************************************************
 * @file    :   motor.c
 * @brief   :   Functions related to controlling the motor
 * @author  :   Davide Campagna
 * @date    :   Oct 13, 2022
 * @version :   V1.0
*******************************************************************************/

#include "main.h"
#include "motor.h"
#include <string.h>

Q_DEFINE_THIS_MODULE("Motor")

static uint8_t speed_table[28u];

static uint8_t substp_from_delta_pwm(motor * const me, uint16_t const delta_pwm, uint8_t * const substp_lsr);

uint32_t configure_motor(motor * const me, QEvt const * const e)
{
	uint32_t err = NO_ERROR;
	motor_cfg_evt const * const pCfgEvt = Q_EVT_CAST(motor_cfg_evt);

	/* Check pre-conditions for this function */
	Q_REQUIRE(me != NULL);
	Q_REQUIRE(pCfgEvt != NULL);

	/* CV#2 - Vstart
	 * 
	 * Currently not implemented.
	 *
	 * "Vstart is used to define the voltage drive level used as the start voltage on the motor. The voltage drive
	 *  levels shall correspond linearly to the voltage applied to the motor at speed step one, as a fraction of
	 *  available rectified supply voltage. When the voltage drive level is equal to zero, there shall be zero voltage
	 *  applied to the motor. When it is at maximum "11111111", the full available rectified voltage shall be applied."
	 */

	/*
	 * CV#3 - Acceleration Rate
	 * CV#4 - Deceleration Rate
	 *
	 * Compute timing of acceleration and acceleration as specified in the NMRA S-9.2.2 document, configuration
	 * variables #3 and #4 (R):
	 * "Determines the decoder's acceleration rate. The formula for the acceleration rate shall be equal to (the
	 * contents of CV#3*.896)/(number of speed steps in use). For example, if the contents of CV#3=2, then the
	 * acceleration is 0.064 sec/step for a decoder currently using 28 speed steps. If the content of this parameter
	 * equals "0" then there is no programmed momentum during acceleration."
	 * 
	 * Actual speed increment/decrement will be calculated on the run.
	 * Length of a step is stored in milliseconds to achieve better accuracy
	 */
	me->accel_t = pCfgEvt->config.acc_rate * 896u / 28u;	/* 896 / 28 = 32 */
	me->decel_t = pCfgEvt->config.dec_rate * 896u / 28u;

	/*
	 * CV#9 - Total PWM Period
	 *
	 * "The value of CV#9 sets the nominal PWM period at the decoder output and therefore the frequency is proportional
	 *  to the reciprocal of the value."
	 *
	 * Instead of using the "reccomended formula", a simpler approach is taken:
	 *
	 * T_pwm = 31.25 * CV#9, or f_pwm = 32KHz / CV#9
	 *
	 * Acceptable values of CV#9 will be between 1 and 64
	 *
	 * PWM period will be in the range [31.25..2000] µs, (frequency in range [500Hz..32kHz])
	 * 
	 */

	uint8_t temp_pwm_t = pCfgEvt->config.pwm_t;

	/* Clamp values inside desired range */
	if (temp_pwm_t < 1u) {
		temp_pwm_t = 1u;
	} else if (temp_pwm_t > 64u) {
		temp_pwm_t = 64u;
	}

	/* Calculate pwm timer ARR (with timer clock @ 32MHz) */
	me->pwm_arr = (1024u * temp_pwm_t) - 1u;

	/*
	 * CV#23 - Acceleration Adjustment
	 * CV#24 - Deceleration Adjustment
	 *
	 * Currently not implemented.
	 *
	 * "This Configuration Variable contains additional acceleration rate information that is to be added to or
	 *  subtracted from the base value contained in Configuration Variable #3 using the formula
	 *  (the contents of CV#23*.896)/(number of speed steps in use). This is a 7 bit value (bits 0-6) with bit 7 being
	 *  reserved for a sign bit (0-add, 1-subtract). In case of overflow the maximum acceleration rate shall be used. In
	 *  case of underflow no acceleration shall be used. The expected use is for changing momentum to simulate differing
	 *  train lengths/loads, most often when operating in a consist."
	 *
	 */

	/* CV#67-94: Speed Table
	 *
	 */
	Q_ASSERT(sizeof(speed_table) == sizeof(pCfgEvt->config.speed_tab));
	/* Copy the speed table read from the CV table to the local RAM table */
	memcpy(speed_table, pCfgEvt->config.speed_tab, sizeof(speed_table));

	me->speed_table = speed_table;
	me->speed_table_len = sizeof(speed_table);


	/* Check post-condition */
	Q_ENSURE(NULL != me->speed_table);
	Q_ENSURE((1023u <= me->pwm_arr) && (me->pwm_arr <= 0xFFFFu));

	return err;
}

uint32_t motor_reset_substeps(motor * const me)
{
	uint32_t err = NO_ERROR;

	/* Check function's pre-conditions */
	Q_REQUIRE(NULL != me);

	me->substp_cnt  = 0u;
	me->substp_num  = 0u;
	me->substp_lsr  = 0u;
	me->delta_pwm   = 0u;
	me->t_substp    = 0u;
	me->next_step   = me->actual_step;
	me->target_step = me->actual_step;

	/* PWM duty cycle is a 10-bit value, adjust speed by shifting left by 2 (multiply by 4) */
	me->actual_pwm = me->speed_table[me->actual_step] << 2u;

	return err;
}

uint32_t compute_substep_param(motor * const me)
{
	uint32_t err = NO_ERROR;
	uint8_t substp_lsr = 0u;

	/* Check function's pre-conditions */
	Q_REQUIRE(NULL != me);

	/* If substeps are at 0, compute and start next sequence */
	if (me->substp_cnt == 0u) {
		if (me->actual_step < me->target_step) {

			me->next_step = me->actual_step + 1u;

			me->delta_pwm = me->speed_table[me->next_step] - me->speed_table[me->actual_step];

			me->substp_num = substp_from_delta_pwm(me, me->delta_pwm, &substp_lsr);

			/* When accelerating, start from actual speed */
			me->substp_cnt = 0u;

			/* ms between substeps bases on acceleration rate */
			me->t_substp = me->accel_t / me->substp_num;

			me->accel_dir = SPEED_INCREASE;

		} else if (me->actual_step > me->target_step) {

			me->next_step = me->actual_step - 1u;

			me->delta_pwm = me->speed_table[me->actual_step] - me->speed_table[me->next_step];

			me->substp_num = substp_from_delta_pwm(me, me->delta_pwm, &substp_lsr);

			/* When decelerating, start from the upper step */
			me->substp_cnt = me->substp_cnt;

			/* ms between substeps bases on deceleration rate */
			me->t_substp = me->decel_t / me->substp_num;

			me->accel_dir = SPEED_DECREASE;

		} else {
			/* Substepping called without an acceleration! */
			Q_ERROR();
		}
	} else {
		/* Else update the actual sequence */
		/* If target is greater than actual step, update the next step */
		if (me->actual_step < me->target_step) {
			me->next_step = me->actual_step + 1u;

			/* Keep substeps counter and number the same as before */

			/* Only update the timer period */
			me->t_substp = me->accel_t / me->substp_num;

			me->accel_dir = SPEED_INCREASE;
		} else {
			/* In this case, if target is the same as the actual, the procedure is the same as for the
			 * deceleration, stepping back towards the actual speed */
			me->next_step = me->actual_step;

			/* Keep substeps counter and number the same as before */

			/* Only update the timer period */
			me->t_substp = me->decel_t / me->substp_num;

			me->accel_dir = SPEED_DECREASE;
		}
	}

	return err;
}

static uint8_t substp_from_delta_pwm(motor * const me, uint16_t const delta_pwm, uint8_t * const substp_lsr)
{
	uint16_t substp_cnt = 1u;
	bool greater = false;

	Q_REQUIRE(delta_pwm != 0u);
	Q_REQUIRE(substp_lsr != NULL);

	/* Check if the number is greater than 16, or 8, ... */
	for (int8_t i = 4u; (i > 1u) && (!greater); i--) {
		if ((delta_pwm >> i) != 0u) {

			/* If greater than 16, divide by 32 (>>5),
			 * if greater than  8, divide by 16 (>>4),
			 * and so on...
			 */
			*substp_lsr = i + 1u;
			substp_cnt = 1u << *substp_lsr;

			greater = true;
		}
	}

	if (!greater) {
		me->substp_lsr = 0u;
	}

	return substp_cnt;
}

bool is_at_speed(motor const * const me)
{
	return (me->actual_step == me->target_step) && (me->substp_cnt == 0u);
}

bool is_stopped(motor const * const me)
{
	return (me->speed_table[me->actual_step] == 0u) && (me->substp_cnt == 0u);
}

void motor_pwm_set(int16_t value)
{
    /* TODO: Write pwm value to the timer register */
}

void motor_dir_set(enum motor_direction dir)
{
    /* TODO: Set the motor driver direction */
}

void brake(bool fast)
{
    /* TODO: Brake the motor */
}

inline void emergency_brake(void)
{
	brake(true);
}
