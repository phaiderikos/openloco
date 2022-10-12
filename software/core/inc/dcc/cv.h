/*******************************************************************************
 * @file    :   cv.h
 * @brief   :   Functions related to Configuration Variables reading and writing
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0
*******************************************************************************/

#ifndef __DCC_CV_H
#define __DCC_CV_H

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#define EEPROM_START_ADDR	DATA_EEPROM_BASE
#define EEPROM_SIZE		512

#define LAST_CV_NUM		128
#define CV_BANK1_START_ADDR		(EEPROM_START_ADDR)
#define CV_BANK1_END_ADDR		(CV_BANK1_START_ADDR + LAST_CV_NUM - 1)
#define CV_BANK1_OK_ADDR		(CV_BANK1_END_ADDR + 1)
#define CV_BANK2_START_ADDR		(CV_BANK1_OK_ADDR + 4)
#define CV_BANK2_END_ADDR		(CV_BANK2_START_ADDR + LAST_CV_NUM - 1)
#define CV_BANK2_OK_ADDR		(CV_BANK2_END_ADDR + 4)

enum cv_op_result {CV_OP_OK, CV_OP_ERROR} ;

/**
 * @brief: Checks if a CV's functionality is implemented.
 * @returns: true if the CV is implemented, false otherwise.
 */
bool is_cv_implemented(uint16_t num);

/**
 * @brief: Copies the value of all CVs from the data EEPROM to the RAM array.
 */
uint8_t reload_all_cvs(void);

/**
 * @brief: Saves all the CVs to the data EEPROM. Usually not needed since the
 * values are kept syncronized between RAM and EEPROM.
 */
uint8_t save_all_cvs(uint8_t bank);

/**
 * @brief Reads the value of a single CV.
 */
uint8_t read_cv(uint16_t num);

/**
 * @brief Set the value of a single CV. The new value is updated both in RAM and
 * in persistent storage.
 */
uint8_t write_cv(uint16_t num, uint8_t val);

/* void init_volatile_cvs(void); */

/**
 * @brief: Sets all the CVs to their factory defaults.
 */
uint8_t reset_cvs();

#endif				/* __DCC_CV_H */
