#include <stdlib.h>
#include "ematch.h"
#include "tasks/command_line_interface/details/internals.h"
#include "recovery.h"
#include "hardware_definitions.h"

OPTION_NEW_TOOL_IMPL(ematch)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    CHECK_ARGUMENTS();

    if(argv[1][0] != '-')
    {
        PRINT("Command [%s] not recognized.\n", ARGUMENTS_STRING);
        return false;
    }

    int option_index = 0;
    optind = 0; /* resetting the optind */
    int temp_optind = optind;
    while (1)
    {
        int opt;
        (EMATCH_ARG_OPTIONS.LONG == NULL)
        ?
        (opt = getopt(argc, argv, EMATCH_ARG_OPTIONS.SHORT))
        :
        (opt = getopt_long(argc, argv, EMATCH_ARG_OPTIONS.SHORT, EMATCH_ARG_OPTIONS.LONG, &option_index));

        if(opt == -1)
            break;

        // saving optind
        temp_optind = optind;

        switch(opt)
        {
            OPT_CASE_FUNC(300, 'h', ematch, h, optarg);
            OPT_CASE_FUNC(301, 'a', ematch, a, optarg);
            OPT_CASE_FUNC(302, 'b', ematch, b, optarg);
            OPT_CASE_FUNC(303, 'c', ematch, c, optarg);
            OPT_CASE_FUNC(304, 'd', ematch, d, optarg);
            OPT_CASE_FUNC(305, 'e', ematch, e, optarg);
            OPT_CASE_FUNC(306, 'f', ematch, f, optarg);
            OPT_CASE_FUNC(307, 'g', ematch, g, optarg);
            OPT_CASE_FUNC(308, 'i', ematch, i, optarg);
            OPT_CASE_FUNC(309, 'j', ematch, j, optarg);

            case '?':
            PRINT("Invalid option [%s] or missing arguments for [%s]\n", ARGUMENTS_STRING, ARGUMENTS_STRING);
                break;
            default:
            PRINT("Command [%s] not recognized.\n", ARGUMENTS_STRING);
                break;
        }

        // restoring optind
        optind = temp_optind;
    }

    return true;
}


OPTION_FUNCTION_IMPL(ematch, h)
{
    uart_transmit_line(__s_uart, EMATCH_USAGE);
    return true;
}

OPTION_FUNCTION_IMPL(ematch, a)
{
    RecoverySelect event = DROGUE;
    RecoveryContinuityStatus cont = recovery_check_continuity(event);
    
    if(cont == OPEN_CIRCUIT)
    {
        sprintf(__s_output, "No continuity was detected on the drogue circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }else
    {
        sprintf(__s_output, "Continuity was detected on the drogue circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(ematch, b)
{
    RecoverySelect event = MAIN;
    RecoveryContinuityStatus cont = recovery_check_continuity(event);
    
    if(cont == OPEN_CIRCUIT)
    {
        sprintf(__s_output, "No continuity was detected on the main circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }else
    {
        sprintf(__s_output, "Continuity was detected on the main circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(ematch, c)
{
    RecoverySelect event = DROGUE;
    RecoveryOverCurrentStatus over = recovery_check_overcurrent(event);
    
    if(over == NO_OVERCURRENT)
    {
        sprintf(__s_output, "No overcurrent was detected on the drogue circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }else
    {
        sprintf(__s_output, "Overcurrent was detected on the drogue circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }
    return true;
}

OPTION_FUNCTION_IMPL(ematch, d)
{
    RecoverySelect event = MAIN;
    RecoveryOverCurrentStatus over = recovery_check_overcurrent(event);
    
    if(over == NO_OVERCURRENT)
    {
        sprintf(__s_output, "No overcurrent was detected on the main circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }else
    {
        sprintf(__s_output, "Overcurrent was detected on the main circuit.\n");
        uart_transmit_line(__s_uart, __s_output);
    }
    return true;
}

OPTION_FUNCTION_IMPL(ematch, e)
{
    RecoverySelect event = DROGUE;
    recovery_enable_mosfet(event);
    sprintf(__s_output, "DROGUE DEPLOYMENT CIRCUIT IS NOW ARMED!.\n");
    uart_transmit_line(__s_uart, __s_output);
    
    return true;
}

OPTION_FUNCTION_IMPL(ematch, f)
{
    RecoverySelect event = MAIN;
    recovery_enable_mosfet(event);
    sprintf(__s_output, "MAIN DEPLOYMENT CIRCUIT IS NOW ARMED!.\n");
    uart_transmit_line(__s_uart, __s_output);
    
    return true;
}

OPTION_FUNCTION_IMPL(ematch, g)
{
    RecoverySelect event = DROGUE;
    
    sprintf(__s_output, "DROGUE WILL FIRE IN %d SECONDS!.\n", __DELAY_EMATCH_MENU_FIRE / 1000);
    uart_transmit_line(__s_uart, __s_output);
    
    int time_left = __DELAY_EMATCH_MENU_FIRE;
    
    while(time_left > 0)
    {
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        time_left -= (1000);
        sprintf(__s_output, "DROGUE WILL FIRE IN %d SECONDS!.\n", time_left / 1000);
        uart_transmit_line(__s_uart, __s_output);
    }
    recovery_activate_mosfet(event);
    
    return true;
}

OPTION_FUNCTION_IMPL(ematch, i)
{
    RecoverySelect event = MAIN;
    
    sprintf(__s_output, "MAIN WILL FIRE IN %d SECONDS!.\n", __DELAY_EMATCH_MENU_FIRE / 1000);
    uart_transmit_line(__s_uart, __s_output);
    
    int time_left = __DELAY_EMATCH_MENU_FIRE;
    
    while(time_left > 0)
    {
        
        vTaskDelay(pdMS_TO_TICKS(1000));
        time_left -= (1000);
        sprintf(__s_output, "MAIN WILL FIRE IN %d SECONDS!.\n", time_left / 1000);
        uart_transmit_line(__s_uart, __s_output);
    }
    recovery_activate_mosfet(event);
    
    return true;
}

OPTION_FUNCTION_IMPL(ematch, j)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    
    // todo: make sure the args are correct
    strcpy(val_str, argv[1]);
    
    int value = atoi(val_str);
    
    if(value > 5 && value < 60)
    {
        
        __DELAY_EMATCH_MENU_FIRE = value * 1000;
        sprintf(__s_output, "E-match fire delay_ms set to %d.\n", value);
        uart_transmit_line(__s_uart, __s_output);
    }
    
    return true;
}
