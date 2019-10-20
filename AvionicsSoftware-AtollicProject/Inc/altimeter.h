#ifndef ALTIMETER_H
#define ALTIMETER_H
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS 2018-2020
//
// Repository:
//  UMSATS Google Drive: UMSATS/Guides and HowTos.../Command and Data Handling (CDH)/Coding Standards
//
// File Description:
//  Template header file for C / C++ projects. Unused sections can be deleted.
//
// History
// 2019-01-13 by Tamkin Rahman
// - Created.

#include <inttypes.h>
#include "forward_declarations.h"

typedef union {

	float float_val;
	uint32_t byte_val;
} alt_value;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Calculate altitude from pressure reading
//
// Parameters:
//  Pressure - [Pa]
//
// Returns:
//  float - float value of altitude approximation
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
alt_value altitude_approx(float pressure, float temperature, configData_t*config);
#endif // TEMPLATE_H
