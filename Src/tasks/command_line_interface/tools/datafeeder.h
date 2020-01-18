//
// Created by vasil on 1/17/2020.
//

#ifndef AVIONICS_DATAFEEDER_H
#define AVIONICS_DATAFEEDER_H

#include "../definitions.h"


OPTION_NEW_TOOL_DECL(datafeeder);
OPTION_FUNCTION_DECL(datafeeder, h);
OPTION_FUNCTION_DECL(datafeeder, s);
OPTION_FUNCTION_DECL(datafeeder, e);
OPTION_FUNCTION_DECL(datafeeder, d);

DECL_NEW_USAGE(EMATCH) = "Data Feeder:\r\n"
                         "\t[s] - start\r\n"
                         "\t[e] - exit\n"
                         "\t[d] - data";


DECL_NEW_ARG_OPTIONS_FOR(EMATCH) =
{
        .LONG = (opt_arg_long_option[])
        {
            OPTION_ADD_OPTION(help, 300),
            OPTION_ADD_OPTION(start, 300),
            OPTION_ADD_OPTION(exit,  301),
            OPTION_ADD_OPTION(data,  302),
            OPTION_LIST_END
        },

        .SHORT = "h::s::e::d",

        .FUNCTIONS = (option_function[])
        {
                OPTION_ADD_FUNCTION(datafeeder, h),
                OPTION_ADD_FUNCTION(datafeeder, s),
                OPTION_ADD_FUNCTION(datafeeder, e),
                OPTION_ADD_FUNCTION(datafeeder, d),
        }
};



#endif //AVIONICS_DATAFEEDER_H
