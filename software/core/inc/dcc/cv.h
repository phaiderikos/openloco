/*******************************************************************************
 * @file    :   cv.h
 * @brief   :   Functions related to Configuration Variables reading and writing
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0
*******************************************************************************/

#ifndef __DCC_CV_H
#define __DCC_CV_H

#include <stdio.h>
#include <stdint.h>

FILE *open_cv_file();

void close_cv_file(void);

uint8_t reload_all_cvs(void);

uint8_t save_all_cvs(void);

uint8_t read_cv(uint16_t num);

uint8_t write_cv(uint16_t num, uint8_t val);

void init_volatile_cvs(void);

void reset_cvs(void);

#endif //__DCC_CV_H
