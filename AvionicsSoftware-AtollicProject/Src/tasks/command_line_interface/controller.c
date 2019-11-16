#ifndef CLI_CONTROLLER_SRC
#define CLI_CONTROLLER_SRC

#include "tasks/command_line_interface/controller.h"
#include "tasks/command_line_interface/details/internals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "tasks/command_line_interface/definitions.h"
//#include "tasks/command_line_interface/tools/configure.h"
//#include "tasks/command_line_interface/tools/ematch.h"
//#include "tasks/command_line_interface/tools/mem.h"
//#include "tasks/command_line_interface/tools/read.h"

#include "external/sensors/bmp3_defs.h"
#include "external/sensors/bmi08x_defs.h"

#include "flash.h"
#include "cmsis_os.h"
#include "recovery.h"
#include "protocols/UART.h"
#include "stm32/STM32.h"
#include "configuration.h"

CREATE_OPT_DEFAULT_FUNCTION(general, default_behaviour)
{
    sprintf(__s_output, "Command [%s] not recognized.", arguments);
    uart_transmit_line(__s_uart, arguments);
    
    return true;
}

CREATE_OPT_ERROR_FUNCTION(general, error_behaviour)
{
    // TODO: do something
}


bool task_command_line_controller_execute_command(const char *command)
{
    EXTRACT_PROGRAM_NAME_AND_ARGUMENTS(command);
    int option_index = 0;

    optind = 0; /* resetting the optind */
    int temp_optind = optind;
    while (1)
    {
        int opt = getopt_long(2, PROGRAM, GENERAL_ARG_OPTIONS.SHORT, GENERAL_ARG_OPTIONS.LONG, &option_index);
        
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
            // Add more commands
            
//            OPT_ERROR_FUNC  (configure, error_behaviour,   command);
//            OPT_DEFAULT_FUNC(configure, default_behaviour, command);
        }
        
        // restoring optind
        optind = temp_optind;
    }
    
    return true;
}



// ----- Case functions implementation ----- //
OPTION_FUNCTION_IMPL(general, help)
{
    uart_transmit_line(__s_uart, GENERAL_USAGE);
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






void task_command_line_controller_init(cli_thread_parameters *_parameters)
{
    if(true == __initialized){
        return;
    }
    
    cli_thread_parameters *parameters = (cli_thread_parameters *) _parameters;
    __application_configurations = parameters->application_configurations;
    __s_uart = parameters->huart;
    __command_buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE); //command buffer
    __state = MAIN_MENU;
    __flash = parameters->flash;
    
    __initialized = true;
}


void thread_command_line_controller_start(void const *pvParameters)
{
    if(false == __initialized){
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    uart_transmit_line(__s_uart, INTRO_USAGE);
    general_help_function(NULL);
    
    /* As per most FreeRTOS tasks, this task is implemented in an infinite loop. */
    while(1)
    {
        uart_transmit(__s_uart, ">> ");
        __command_buffer = uart_receive_command(__s_uart); //puts input into buffrx
        task_command_line_controller_execute_command(__command_buffer); //handles command sitting in buffrx
    }
}

void task_command_line_controller_println(char *line)
{
    uart_transmit_line(__s_uart, line);
}






#endif // CLI_CONTROLLER_SRC

