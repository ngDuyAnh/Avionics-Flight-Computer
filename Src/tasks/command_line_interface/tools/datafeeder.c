//
// Created by vasil on 1/17/2020.
//

#include <utilities/common.h>
#include "datafeeder.h"
#include "tasks/command_line_interface/details/internals.h"

CREATE_OPT_DEFAULT_FUNCTION(datafeeder, default_behaviour)
{

    return true;
}


CREATE_OPT_ERROR_FUNCTION(datafeeder, error_behaviour)
{
    // TODO: do something
}



OPTION_NEW_TOOL_IMPL(datafeeder)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
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
            OPT_CASE_FUNC(300, 'h', datafeeder, h, optarg);
            OPT_CASE_FUNC(301, 's', datafeeder, s, optarg);
            OPT_CASE_FUNC(302, 'e', datafeeder, e, optarg);

            // Add more commands
            OPT_ERROR_FUNC  (datafeeder,  error_behaviour,   ARGUMENTS_STRING);
            OPT_DEFAULT_FUNC(datafeeder, default_behaviour,  ARGUMENTS_STRING);
        }

        // restoring optind
        optind = temp_optind;
    }

    return true;
}


OPTION_FUNCTION_IMPL(datafeeder, h)
{
    uart_transmit_line(__s_uart, EMATCH_USAGE);
    return true;
}

OPTION_FUNCTION_IMPL(datafeeder, s)
{
    return true;
}


OPTION_FUNCTION_IMPL(datafeeder, e)
{
    return true;
}

OPTION_FUNCTION_IMPL(datafeeder, d)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING)
    const char * data = argv[1];
    uint32_t timestamp = read_32(&data[0]);
    uint32_t accel[3] = {read_32(&data[4]), read_32(&data[8]), read_32(&data[12])};
    uint32_t gyro [3] = {read_32(&data[16]), read_32(&data[20]), read_32(&data[24])};
    uint32_t pressure = read_32(&data[28]);
    uint32_t temperature = read_32(&data[32]);

    // update sensors readings

    return true;
}


