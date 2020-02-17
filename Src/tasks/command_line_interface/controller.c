#ifndef CLI_CONTROLLER_SRC
#define CLI_CONTROLLER_SRC

#include "controller.h"
#include "tasks/command_line_interface/details/internals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "definitions.h"
//#include "tasks/command_line_interface/tools/configure.h"
//#include "tasks/command_line_interface/tools/ematch.h"
//#include "tasks/command_line_interface/tools/mem.h"
//#include "tasks/command_line_interface/tools/read.h"


bool __initialized                                  = false;
void* __startupTaskHandle                           = NULL;
char *__command_buffer                              = NULL;
menuState_t __state                                 = MAIN_MENU;
uint16_t __DELAY_EMATCH_MENU_FIRE                   = 10000;
configuration_data_t * __application_configurations = NULL;
char __s_output[256]                                = {};


#include "external/sensors/bmp3_defs.h"
#include "external/sensors/bmi08x_defs.h"

#include "flash.h"
#include "cmsis_os.h"
#include "recovery.h"
#include "protocols/UART.h"
#include "stm32/STM32.h"
#include "configuration.h"


bool task_command_line_controller_execute_command(const char *command)
{
    EXTRACT_PROGRAM_NAME_AND_ARGUMENTS(command);
    int option_index = 0;

    optind = 0; /* resetting the optind */
    int temp_optind = optind;
    while (1)
    {
        int opt = getopt_long_only(2, PROGRAM, GENERAL_ARG_OPTIONS.SHORT, GENERAL_ARG_OPTIONS.LONG, &option_index);

        if(opt == -1)
            break;

        // saving optind
        temp_optind = optind;

        switch(opt)
        {
            OPT_CASE_FUNC(302, 'h', general, help, ARGUMENTS);
            OPT_CASE_FUNC(305, 's', general, save, ARGUMENTS);
            OPT_CASE_FUNC(306, 'S', general, start, ARGUMENTS);


            OPT_CASE_MODULE(300, 'c', configure, ARGUMENTS);
            OPT_CASE_MODULE(301, 'e', ematch,    ARGUMENTS);
            OPT_CASE_MODULE(303, 'm', mem,       ARGUMENTS);
            OPT_CASE_MODULE(304, 'r', read,      ARGUMENTS);
            OPT_CASE_MODULE(307, 'd', datafeeder,ARGUMENTS);
            OPT_CASE_FUNC(308, 't', general, test, ARGUMENTS);


            // Add more commands
            case '?':
                PRINT("Command [%s] not recognized.\n", command);
                general_help_function(NULL);
                break;
            default:
                PRINT("Command [%s] not recognized.\n", command);
                general_help_function(NULL);
                break;
        }

        // restoring optind
        optind = temp_optind;
    }

    return true;
}




// ----- Case functions implementation ----- //
OPTION_FUNCTION_IMPL(general, help)
{
    uart6_transmit_line(GENERAL_USAGE);
    return true;
}

OPTION_FUNCTION_IMPL(general, save)
{
    write_config(__application_configurations);
    return true;
}

OPTION_FUNCTION_IMPL(general, start)
{
    __application_configurations->values.state = STATE_LAUNCHPAD_ARMED;
    vTaskResume(__startupTaskHandle);
    vTaskSuspend(NULL);
    
    return true;
}


OPTION_FUNCTION_IMPL(general, test)
{
    return true;
}







void task_command_line_controller_init(cli_thread_parameters *_parameters)
{
    if(true == __initialized){
        return;
    }
    
    cli_thread_parameters *parameters = (cli_thread_parameters *) _parameters;
    __application_configurations = parameters->application_configurations;
    __command_buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE); //command buffer
    __state = MAIN_MENU;

    __initialized = true;
}


void thread_command_line_controller_start(void const *pvParameters)
{
    if(false == __initialized){
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    uart6_transmit_line(INTRO_USAGE);
    general_help_function(NULL);
    
    /* As per most FreeRTOS tasks, this task is implemented in an infinite loop. */
    while(1)
    {
        uart6_transmit(">> ");
        __command_buffer = uart6_receive_command(); //puts input into buffrx
        task_command_line_controller_execute_command(__command_buffer); //handles command sitting in buffrx
    }
}

void task_command_line_controller_println(char *line)
{
    uart6_transmit_line(line);
}






#endif // CLI_CONTROLLER_SRC

