// DON'T TOUCH AND DO NOT INCLUDE ANYWHERE EXCEPT FOR THE NEW CLI TOOLS ADDED

#ifndef CLI_INTERNALS_H
#define CLI_INTERNALS_H

#include "../controller.h"
                                                           ;
extern bool __initialized                                  ;
extern UART __s_uart                                       ;
extern void* __startupTaskHandle                           ;
extern char *__command_buffer                              ;
extern menuState_t __state                                 ;
extern Flash __flash                                       ;
extern uint16_t __DELAY_EMATCH_MENU_FIRE                   ;
extern configuration_data_t * __application_configurations ;
extern char __s_output[256]                                ;


#endif // CLI_INTERNALS_H

