/*******************************************************************************
 * @file    :   cv.c
 * @brief   :   Functions related to Configuration Variables reading and writing
 * @author  :   Davide Campagna
 * @date    :   Sep 09, 2022
 * @version :   V1.0a
*******************************************************************************/

/**
 * WARNING: this file is subjected to changes and expansions that can result in
 * the loss of the data saved inside the CVs. A firmware update could render all
 * the CVs inside the EEPROM inusable, for example in the event of a change in
 * how they are stored.
 */

#include "cv.h"
#include "config.h"


#include <string.h>

/**
 *
 * Mandatory (M), Recommended (R) or Optional (O).
 *
 * A "U" (Uniform Spec) indicates a CV which requires implementation by
 * manufacturers according to a common specification.
 *
 * Dinamic CVs are marked with a "D"
 *
 *
 * 1 Primary Address		M U
 * 2 Vstart         		R
 * 3 Acceleration Rate		R
 * 4 Deceleration Rate		R
 * 5 Vhigh			O
 * 6 Vmid			O
 * 7 Manufacturer Version No.   M RO
 * 8 Manifactured ID		M RO U
 * 9 Total PWM Period		O
 * 10 EMF Feedback Cutout	O
 * 11 PAcket Time-Out Value	R
 * 12 Power Source Conversion	O U
 * 13 Alt. Mode Func. Status  F1-F8		O U
 * 14 Alt. Mode Func. 2 Status FL,F9-F12	O U
 * 15,16 Decoder Lock		O U
 * 17-18 Extended Address	O U
 * 19 Consist Address		O U
 * 20 RESERVED FOR NMRA
 * 21 Consist addr Active F1-F8			O U
 * 22 Consist addr Active FL,F9-F12		O U
 * 23 Acceleration Adjustment			O U
 * 24 Deceleration Adjustment			O U
 * 25 Speed Table/Mid-range Cab Speed Step	O U
 * 26 RESERVED FOR NMRA
 * 27 Decoder Automatic Stopping Configuration	O U
 * 28 Bi-Directional Communication		O U
 * 29 Configuration Data #1			M U
 * 30 Error Information				O U
 * 31 Index High Byte				O U
 * 32 Index Low Byte				O U
 * 33-46 Output Location FL(f),FL(r),F1-F12	O U
 * 37-64 Manifacturer Unique
 * 65 Kick Start		O
 * 66 Forward Trim		O
 * 67-94 Speed Table		O
 * 95 Reverse Trim		O
 * 96-104 RESERVED FOR NMRA
 * 105 User Identifier #1	O
 * 106 User Identifier #2	O
 * 107-111 RESERVED FOR NMRA
 * 112-256 Manifacturer Unique	O
 * 257-512 Indexed Area
 * 513-879 RESERVED FOR NMRA
 * 880-891 RESERVED FOR NMRA
 * 892 Decoder Load 		O U D
 * 893 Dynamic Flags    	O U D
 * 894 Fuel/Coal		O U D
 * 895 Water			O U D
 * 896-1024 SUSI Sound and Function Modules
 */

/**
 * Memory layout:
 * 0-127: CV#1 -> CV#128
 */

uint8_t CV[LAST_CV_NUM];

static bool ram_only = false;

// TODO: check the result of every function call about data EEPROM

uint8_t reset_cvs(void)
{
	ram_only = true;

	memset(CV, 0x00, LAST_CV_NUM);

	/* init_volatile_cvs(); */

	/* Initializing variables with theire default values */
	write_cv(1, DCC_ADDRESS);
	write_cv(29, CV29);

	ram_only = false;

	/* Erase CVs in data EEPROM */
	return (save_all_cvs(1) == CV_OP_OK && save_all_cvs(2) == CV_OP_OK) ? CV_OP_OK : CV_OP_ERROR;
}

uint8_t reload_all_cvs()
{
	bool bank1_ok, bank2_ok;
	uint32_t *ok_bank, *error_bank;

	bank1_ok = *((uint32_t *) CV_BANK1_OK_ADDR) == 0xaabbccdd;
	bank2_ok = *((uint32_t *) CV_BANK2_OK_ADDR) == 0xaabbccdd;

	if (bank1_ok) {
		ok_bank = (uint32_t *) CV_BANK1_OK_ADDR;
		if (bank2_ok)
			error_bank = 0;
		else
			error_bank = (uint32_t *)CV_BANK2_OK_ADDR;
	} else {
		if (bank2_ok) {
			error_bank = (uint32_t *) CV_BANK1_OK_ADDR;
			ok_bank = (uint32_t *) CV_BANK2_OK_ADDR;
		} else {
			/* Both banks have problems */
			ok_bank = 0;
			error_bank = 0;
			return reset_cvs();
		}
	}

	/* Only load into ram if at least a bank was valid */
	if (ok_bank) {
		/* Load CVs into RAM */
		for (uint16_t i = 0; i < LAST_CV_NUM; i++)
			CV[i] = *((uint8_t *) ok_bank + i);
		if (error_bank) {
			return save_all_cvs((error_bank == (uint32_t *) CV_BANK1_START_ADDR) ? 1 : 2);
		}
	}

	return CV_OP_OK;
}

uint8_t read_cv(uint16_t num)
{
	if (is_cv_implemented(num)) {
		/* CVs array is kept in sync with data EEPROM from startup,
		   there is no need to read from EEPROM every time */
		return CV[num - 1];
	} else {
		return 0;
	}
}

uint8_t write_cv(uint16_t num, uint8_t val)
{
	if (is_cv_implemented(num)) {
		/* CV#7 and CV#8 are read only */
		if (num == 7 || num == 8)
			return 0;

		CV[num - 1] = val;

		if (!ram_only) {
			HAL_FLASHEx_DATAEEPROM_Unlock();

			uint32_t offset = (num - 1) & ~0x03;

			/* Invalidate bank1 */
			HAL_FLASHEx_DATAEEPROM_Erase(CV_BANK1_OK_ADDR);

			/* Erase word around that CV */
			HAL_FLASHEx_DATAEEPROM_Erase(CV_BANK1_START_ADDR + offset);

			/* Rewrite whole word at the correct location */
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, CV_BANK1_START_ADDR + offset, *((uint32_t *)(CV + offset)));

			/* Write validation word of bank1 */
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, CV_BANK1_OK_ADDR, 0xaabbccdd);


			/* Invalidate bank2 */
			HAL_FLASHEx_DATAEEPROM_Erase(CV_BANK2_OK_ADDR);

			/* Erase word around that CV */
			HAL_FLASHEx_DATAEEPROM_Erase(CV_BANK2_START_ADDR + offset);

			/* Rewrite whole word at the correct location */
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, CV_BANK2_START_ADDR + offset, *((uint32_t *)(CV + offset)));

			/* Write validation word of bank2 */
			HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, CV_BANK2_OK_ADDR, 0xaabbccdd);

			HAL_FLASHEx_DATAEEPROM_Lock();
		}

		return CV_OP_OK;
	} else {
		return CV_OP_ERROR;
	}
}

uint8_t save_all_cvs(uint8_t bank)
{
	uint32_t ok_addr, start_addr;

	if (bank == 1) {
		ok_addr = CV_BANK1_OK_ADDR;
		start_addr = CV_BANK1_START_ADDR;
	} else if (bank == 2) {
		ok_addr = CV_BANK2_OK_ADDR;
		start_addr = CV_BANK2_START_ADDR;
	} else {
		return 1;
	}

	HAL_FLASHEx_DATAEEPROM_Unlock();

	HAL_FLASHEx_DATAEEPROM_Erase(ok_addr);

	for (int i = 0; i < LAST_CV_NUM; i += 4) {
		HAL_FLASHEx_DATAEEPROM_Erase(start_addr + i);

		HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, start_addr + i, *((uint32_t *)(CV + i)));
	}

	HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_WORD, ok_addr, 0xaabbccdd);

	HAL_FLASHEx_DATAEEPROM_Lock();

	return CV_OP_OK;
}

/* void init_volatile_cvs(void)
{

} */

bool is_cv_implemented(uint16_t num)
{
	/* Primary Address */
	if (num == 1)
		return true;

	/* Acceleration & Deceleration Rate */
	if (num == 3 || num == 4)
		return true;

	/* Manufacturer Version No. & Manufactured ID */
	if (num == 7 || num == 8)
		return true;

	/* Alternate Mode Function Status F1-F8 & FL,F9-F12 */
	if (num == 13 || num == 14)
		return true;

	/* Extended Address */
	if (num == 17 || num == 18)
		return true;

	/* Decoder Automatic Stopping Configuration */
	if (num == 27)
		return true;

	/* Output Locations 1-14 for Functions FL(f), FL(r), and F1-F12 */
	if (num >= 33 && num <= 46)
		return true;

	/* Kick Start */
	if (num == 65)
		return true;

	/* Speed Table */
	if (num >= 67 && num <= 94)
		return true;

	return false;
}
