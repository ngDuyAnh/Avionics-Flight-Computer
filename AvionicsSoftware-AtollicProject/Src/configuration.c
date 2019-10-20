//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS Rocketry 2019
//
// Repository:
//  UMSATS/Avionics-2019
//
// File Description:
// Header file for the configuration functions.
//
// History
// 2019-05-26 by Joseph Howarth
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef CONFIG_H
#define CONFIG_H
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------------------------------------------------------------------------------------------------


#include "configuration.h"
#include "bmi08x_defs.h"
#include "bmp3_defs.h"
#include "flash.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINITIONS AND MACROS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// ENUMS AND ENUM TYPEDEFS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTS AND STRUCT TYPEDEFS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// TYPEDEFS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTION PROTOTYPES
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Enter description for static function here.
//
// Returns:
//  Enter description of return values (if any).
//-------------------------------------------------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
configStatus_t init_config(configData_t* configuration){

	configuration->values.id = ID;

	configuration->values.initial_time_to_wait = INITIAL_WAIT_TIME;
	configuration->values.data_rate = DATA_RATE;
	configuration->values.flags = FLAGS;
	configuration->values.start_data_address = DATA_START_ADDRESS;
	configuration->values.end_data_address = DATA_END_ADDRESS;

	configuration->values.ac_bw = ACC_BANDWIDTH;
	configuration->values.ac_odr= ACC_ODR;
	configuration->values.ac_range = ACC_RANGE;
	configuration->values.ac_pwr = ACC_PWR;

	configuration->values.gy_bw = GYRO_BANDWIDTH;
	configuration->values.gy_odr = GYRO_ODR;
	configuration->values.gy_range = GYRO_RANGE;
	configuration->values.gy_pwr = GYRO_PWR;

	configuration->values.bmp_odr = BMP_ODR;
	configuration->values.temp_os = TEMP_OS;
	configuration->values.pres_os = PRES_OS;
	configuration->values.iir_coef = BMP_IIR;

	configuration->values.ref_alt = GND_ALT;
	configuration->values.ref_pres = GND_PRES;

	configuration->values.state = STATE_LAUNCHPAD;

	configStatus_t result = CONFIG_OK;

	return result;

}

configStatus_t read_config(configData_t* configuration){

	configStatus_t stat = CONFIG_ERROR;

	FlashStatus result = flash_read_page(configuration->values.flash,0x00000000,configuration->bytes,sizeof(configData_t)-(sizeof(Flash*)+4)); //The state variable is padded to 4 bytes!

	if(result == FLASH_OK){
		stat = CONFIG_OK;
	}

	return stat;
}

configStatus_t write_config(configData_t* configuration){


	FlashStatus result;

	configStatus_t stat = CONFIG_ERROR;

	result = flash_erase_param_sector(configuration->values.flash,0x00000000);
	while(FLASH_IS_DEVICE_BUSY(flash_get_status_register(configuration->values.flash))){}

	if(result == FLASH_OK){
	 result = flash_program_page(configuration->values.flash,0x00000000,configuration->bytes,sizeof(configData_t)-(sizeof(Flash*)+4));

		while(FLASH_IS_DEVICE_BUSY(flash_get_status_register(configuration->values.flash))){
			stat = CONFIG_OK;
		}
	}

	return stat;
}

#endif
