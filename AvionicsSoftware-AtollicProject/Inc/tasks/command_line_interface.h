#ifndef XTRACT_H
#define XTRACT_H
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS 2018-2020
//
// Repository:
//  UMSATS>Avionics-2019
//
// File Description:
//  xtraxt.h UART CLI utility to pull data off of STM32 flash memory
//
// History
// 2019-02-15 by Eric Kapilik
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "flash.h"
#include "protocols/UART.h"

typedef enum{
	MAIN_MENU,
	READ_MENU,
	CONFIG_MENU,
	EMATCH_MENU,
	MEM_MENU,
	SAVE_MENU
} menuState_t;

typedef struct
{
	UART huart;
	Flash flash;
	configuration_data_t *flightCompConfig;
	void* startupTaskHandle;

}cli_thread_parameters;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  create task to read & handle UART connection
//
// Returns:
//   VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void thread_command_line_interface_start(void const *pvParameters);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  handle command passed in
//		- does not create command parameter deep copy
//		- will call one of the other commands declared below
//
// Returns:
//  VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void task_cli_execute_command(char * command, cli_thread_parameters * params, menuState_t* state);

#endif // XTRACT_H
