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

#include "configuration.h"
#include "external/sensors/bmi08x_defs.h"
#include "external/sensors/bmp3_defs.h"

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
ConfigStatus init_config(configuration_data_t* configuration)
{
    configuration->values.id                    = ID;

    configuration->values.initial_time_to_wait  = INITIAL_WAIT_TIME;
    configuration->values.data_rate             = DATA_RATE;
    configuration->values.flags                 = FLAGS;
    configuration->values.start_data_address    = DATA_START_ADDRESS;
    configuration->values.end_data_address      = DATA_END_ADDRESS;

//    configuration->values.ac_bw                 = ACC_BANDWIDTH;
//    configuration->values.ac_odr                = ACC_ODR;
//    configuration->values.ac_range              = ACC_RANGE;
//    configuration->values.ac_pwr                = ACC_PWR;
//
//    configuration->values.gy_bw                 = GYRO_BANDWIDTH;
//    configuration->values.gy_odr                = GYRO_ODR;
//    configuration->values.gy_range              = GYRO_RANGE;
//    configuration->values.gy_pwr                = GYRO_PWR;
//
//    configuration->values.bmp_odr               = BMP_ODR;
//    configuration->values.temp_os               = TEMP_OS;
//    configuration->values.pres_os               = PRES_OS;
//    configuration->values.iir_coef              = BMP_IIR;
    
    configuration->values.state                 = STATE_LAUNCHPAD;
    
    ConfigStatus result                         = CONFIG_OK;

    return result;

}

ConfigStatus read_config(configuration_data_t* configuration){

    ConfigStatus stat = CONFIG_ERROR;

    FlashStatus result = flash_read(0x00000000, configuration->bytes,
									sizeof(configuration_data_t) - (4)); //The state variable is padded to 4 bytes!
    
    if(result == FLASH_OK)
    {
        stat = CONFIG_OK;
    }

    return stat;
}

ConfigStatus write_config(configuration_data_t* configuration){


    FlashStatus result;

    ConfigStatus stat = CONFIG_ERROR;

    result = flash_erase_param_sector(0x00000000);
    
    while(FLASH_IS_DEVICE_BUSY(flash_get_status_register()))
    {
        ;
    }

    if(result == FLASH_OK)
    {
        result = flash_write(0x00000000, configuration->bytes,
							 sizeof(configuration_data_t) - 4);
        while(FLASH_IS_DEVICE_BUSY(flash_get_status_register()))
        {
            stat = CONFIG_OK;
        }
    }

    return stat;
}