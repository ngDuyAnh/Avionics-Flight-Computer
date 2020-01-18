#ifndef COMMAND_CONTROLLER_H
#define COMMAND_CONTROLLER_H

#include "definitions.h"
#include "flash.h"
#include "protocols/UART.h"
#include "configuration.h"


DECL_NEW_USAGE(INTRO) =   "========== Welcome to Xtract ==========\r\n"
                          "This is a command line interface tool made by the Avionics subdivision of the Rockets team.\r\n\r\n"
                          "Here are some commands to get you started:";
                          
DECL_NEW_USAGE(GENERAL) = "Commands:\r\n"
                          "\t[help] - displays the help menu and more commands\r\n"
                          "\t[read] - Downloads flight data\r\n"
                          "\t[configure] - Setup flight computer\r\n"
                          "\t[ematch] - check and fire ematches\r\n"
                          "\t[mem] - Check on and erase the flash memory\r\n"
                          "\t[save] - Save all setting to the flight computer\r\n"
                          "\t[start] - Start the flight computer\r\n"
                          "\t[datafeeder] - datafeeder event loop";

typedef enum{
    MAIN_MENU,
    READ_MENU,
    CONFIG_MENU,
    EMATCH_MENU,
    MEM_MENU,
    SAVE_MENU
}menuState_t;

typedef struct
{
    UART huart;
    Flash flash;
    configuration_data_t *application_configurations;
    void* startupTaskHandle;

}cli_thread_parameters;

OPTION_FUNCTION_DECL(general, help);
OPTION_FUNCTION_DECL(general, save);
OPTION_FUNCTION_DECL(general, start);

DECL_NEW_ARG_OPTIONS_FOR(GENERAL) =
{
    .LONG = (opt_arg_long_option[])
    {
        OPTION_ADD_OPTION(configure, 300),
        OPTION_ADD_OPTION(ematch,    301),
        OPTION_ADD_OPTION(help,      302),
        OPTION_ADD_OPTION(mem,       303),
        OPTION_ADD_OPTION(read,      304),
        OPTION_ADD_OPTION(save,      305),
        OPTION_ADD_OPTION(start,     306),
        OPTION_ADD_OPTION(datafeeder,307),
        
        OPTION_LIST_END
    },
    
    .SHORT = "c:e:h::m::r::s::S::d",
    
    .FUNCTIONS = (option_function[])
    {
        OPTION_ADD_FUNCTION(general, help),
        OPTION_ADD_FUNCTION(general, save),
        OPTION_ADD_FUNCTION(general, start)
    }
};




void task_command_line_controller_init            (cli_thread_parameters    *_parameters);
void thread_command_line_controller_start         (void const               *pvParameters);
bool task_command_line_controller_execute_command (const char               *command);
void task_command_line_controller_println         (char                     *line);


#endif // COMMAND_CONTROLLER_H
