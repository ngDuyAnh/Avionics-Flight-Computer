//
// Created by vasil on 1/17/2020.
//

#ifndef AVIONICS_DATAFEEDER_H
#define AVIONICS_DATAFEEDER_H

#include "../definitions.h"


OPTION_NEW_TOOL_DECL(datafeeder);
OPTION_FUNCTION_DECL(datafeeder, h);
OPTION_FUNCTION_DECL(datafeeder, s);

DECL_NEW_USAGE(DATAFEEDER) = "Data Feeder:\r\n"
                             "\t[s] - start\r\n"
                             "\tArguments passed as a packet with [timestamp(1), acc(3), gyro(3), pressure(1), temperature(1)]\r\n"
                             "\tRunning till received \'--exit\'\r\n";


DECL_NEW_ARG_OPTIONS_FOR(DATAFEEDER) =
{
        .LONG = (opt_arg_long_option[])
        {
            OPTION_ADD_OPTION(help,  300),
            OPTION_ADD_OPTION(start, 301),
            OPTION_LIST_END
        },

        .SHORT = "h::d:",

        .FUNCTIONS = (option_function[])
        {
                OPTION_ADD_FUNCTION(datafeeder, h),
                OPTION_ADD_FUNCTION(datafeeder, s),
        }
};




#endif //AVIONICS_DATAFEEDER_H
