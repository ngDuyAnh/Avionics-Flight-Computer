//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS 2018-2020
//
// Repository:
//  UMSATS > Avionics 2019
//
// File Description:
//  Altimeter readings interface
//
// History
// 2019-04-01 by Eric Kapilik
// - Created.

#include "altimeter.h"
#include "configuration.h"
#include "utilities/math.h"

alt_value altitude_approx(float pressure, float temperature,configData_t * config)
{
	alt_value result;
	
	result.float_val = math_approximate_altitude(pressure, temperature,
		config->values.ref_pres, config->values.ref_alt);
	
	return result;
}

