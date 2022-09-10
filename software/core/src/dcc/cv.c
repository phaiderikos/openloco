/*******************************************************************************
 * @file    :   cv.c
 * @brief   :   Functions related to Configuration Variables reading and writing
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0
*******************************************************************************/

#include "cv.h"
#include "config.h"

#include <string.h>
#include <errno.h>

/**
 * 
 * Mandatory (M), Recommended (R) or Optional (O).
 * 
 * A "Y" in the Uniform Spec column indicates a CV which requires implementation
 * by manufacturers according to a common specification.
 * 
 * 
 */

static uint8_t CV[1024] = {
    CV01,   // 1 Primary Address        M U
    0,  // 2 Vstart         R
    0,  // 3 Acceleration Rate      R
    0,  // 4 Deceleration Rate      R
    0,  // 5 Vhigh          O
    0,  // 6 Vmid           O
    0,  // 7 Manufacturer Version No.   M RO
    0,  // 8 Manifactured ID        M RO U
    0,  // 9 Total PWM Period       O
    0,  // 10 EMF Feedback Cutout   O
    0,  // 11 PAcket Time-Out Value R
    0,  // 12 Power Source Conversion   O U
    0,  // 13 AM Func. Status F1-F8 O U
    0,  // 14 AM Func. Status FL,F9-F12 O U
    0, 0,   // 15,16 Decoder Lock       O U
    0, 0,   // 17-18 Extended Address   O U
    0,  // 19 Consist Address       O U
    0,  // 20 RESERVED FOR NMRA
    0,  // 21 Consist addr Active F1-F8     O U
    0,  // 22 Consist addr Active FL,F9-F12 O U
    0,  // 23 Acceleration Adjustment       O U
    0,  // 24 Deceleration Adjustment       O U
    0,  // 25 Speed Table/Mid-range Cab Speed Step  O U
    0,  // 26 RESERVED FOR NMRA
    0,  // 27 Decoder Automatic Stopping Configuration  O U
    0,  // 28 Bi-Directional Communication      O U
    CV29,   // 29 Configuration Data #1 M U
    0,  // 30 Error Information     O U
    CV31,   // 31 Index High Byte       O U
    CV32,   // 32 Index Low Byte        O U
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0,   // 33-46 Output Loc. FL(f),LF(r),F1-F12     O U
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0,   // 37-64 Manifacturer Unique    O
    0,  // 65 Kick Start        O
    0,  // 66 Forward Trim      O
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, // 67-94 Speed Table    O
    0,  // 95 Reverse Trim      O
    0, 0, 0, 0,
    0, 0, 0, 0, 0, // 96-104 RESERVED FOR NMRA
    0,  // 105 User Identifier #1   O
    0,  // 106 User Identifier #2   O
    0, 0, 0, 0, 0, // 107-111 RESERVED FOR NMRA
};

/**
 * 112-256 Manifacturer Unique      O
 * 257-512 Indexed Area
 * 513-879 RESERVED FOR NMRA
 * 880-891 RESERVED FOR NMRA
 */

/**
 * 896-1024 SUSI Sound and Function Modules
 */

// TODO: implement
void reset_cvs(void)
{
	memset(CV, 0x00, 1024);
	init_volatile_cvs();

	CV[28] = CV29;
	CV[30] = CV31;
	CV[31] = CV32;

	// clear flash memory
}

// TODO: implement
uint8_t reload_all_cvs(void)
{
	return 0;
}

// TODO: implement
uint8_t read_cv(uint16_t num)
{
	uint8_t tmp;

	tmp = 0;

	return tmp;
}

// TODO: implement
uint8_t write_cv(uint16_t num, uint8_t val)
{
	return val;
}

// TODO: implement
uint8_t save_all_cvs(void)
{
    return 0;
}

void init_volatile_cvs(void)
{
    CV[891] = 0x00; // 892 Decoder Load O U V
    CV[892] = 0x00; // 893 Dynamic Flags    O U V
    CV[893] = 0x00; // 894 Fuel/Coal    O U V
    CV[894] = 0x00; // 895 Water        O U V
}
