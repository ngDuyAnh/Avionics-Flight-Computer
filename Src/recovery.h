
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS Rocketry 2019
//
// Repository:
//  UMSATS/Avionics
//
// File Description:
//  Header file for functions related to recovery ignition circuit.
//
// History
// 2019-05-29 by Joseph Howarth
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef RECOVERY_H
#define RECOVERY_H
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <inttypes.h>

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINITIONS AND MACROS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#define EMATCH_ON_TIME                50

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// ENUMS AND ENUM TYPEDEFS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
typedef enum
{
    DROGUE,
    MAIN
} RecoverySelect;

typedef enum
{
    OPEN_CIRCUIT,
    SHORT_CIRCUIT
} RecoveryContinuityStatus;

typedef enum
{
    NO_OVERCURRENT,
    OVERCURRENT
} RecoveryOverCurrentStatus;

// Group a time stamp with the reading of continuity
typedef struct continuity_data
{
    uint32_t time_ticks; // Time of the reading
    uint8_t continuity_reading; // The status of continuity SHORT_CIRCUIT or OPEN_CIRCUIT
} continuity_data;

// Group a time stamp with the reading of overcurrent
typedef struct overcurrent_data
{
    uint32_t time_ticks; // Time of the reading
    uint8_t overcurrent_reading; // The status of continuity NO_OVERCURRENT or OVERCURRENT
} overcurrent_data;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Sets up the GPIO pins for the recovery functions.
//
// Returns:
//  Enter description of return values (if any).
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void recovery_init();
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Enables the mosfet driver for the specified recovery event .
//
// Returns:
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void recovery_enable_mosfet(RecoverySelect recov_event);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Activates the mosfet driver for the specified recovery event.
//        The driver will be activated for the number of ms specified by the constant EMATCH_ON_TIME.
//  The driver will be disabled after and must be re-enabled be fore every call of this function.
//
// Returns:
//
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void recovery_activate_mosfet(RecoverySelect recov_event);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Checks the continuity for the specified recovery circuit .
//
// Returns:
//        A RecoveryContinuityStatus of OPEN_CIRCUIT or SHORT_CIRCUIT.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
RecoveryContinuityStatus recovery_check_continuity(RecoverySelect recov_event);
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Checks the overcurrent flag for the specified recovery circuit .
//
// Returns:
//        A RecoveryContinuityStatus of NO_OVERCURRENT or OVERCURRENT.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
RecoveryOverCurrentStatus recovery_check_overcurrent(RecoverySelect recov_event);

#endif // RECOVERY_H
