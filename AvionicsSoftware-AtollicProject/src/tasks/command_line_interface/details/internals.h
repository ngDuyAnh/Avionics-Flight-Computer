// DON'T TOUCH AND DO NOT INCLUDE ANYWHERE EXCEPT FOR THE NEW CLI TOOLS ADDED

#ifndef CLI_INTERNALS_H
#define CLI_INTERNALS_H

#include "../controller.h"

static bool __initialized                                  = false;
static UART __s_uart                                       = NULL;
static void* __startupTaskHandle                           = NULL;
static char *__command_buffer                              = NULL;
static menuState_t __state                                 = MAIN_MENU;
static Flash __flash                                       = NULL;
static uint16_t __DELAY_EMATCH_MENU_FIRE                   = 10000;
static configuration_data_t * __application_configurations = NULL;
static char __s_output[256]                                = {};


#endif // CLI_INTERNALS_H

