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
#include "utilities/math.h"

alt_value altitude_approx(float pressure, float temperature, float ref_pres, float ref_alt)
{
	alt_value result;
	result.float_val = math_approximate_altitude(pressure, temperature, ref_pres, ref_alt);
	return result;
}

