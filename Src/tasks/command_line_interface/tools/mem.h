#ifndef AVIONICS_MEM_H
#define AVIONICS_MEM_H

#include "../definitions.h"

OPTION_NEW_TOOL_DECL(mem);
OPTION_FUNCTION_DECL(mem, h);
OPTION_FUNCTION_DECL(mem, a);
OPTION_FUNCTION_DECL(mem, b);
OPTION_FUNCTION_DECL(mem, c);
OPTION_FUNCTION_DECL(mem, d);
OPTION_FUNCTION_DECL(mem, e);

DECL_NEW_USAGE(MEMORY_MENU) = "Commands:\r\n"
                              "\t[h] - displays the help menu and more commands\r\n"
                              "\t[a] - Read 256 bytes (hex address 0-7FFFFF).\r\n"
                              "\t[b] - Scan Memory\r\n"
                              "\t[c] - Erase data section\r\n"
                              "\t[d] - Erase config section.\r\n"
                              "\t[e] - Erase all flash memory.\r\n";



DECL_NEW_ARG_OPTIONS_FOR(MEMORY_MENU) =
{
    .LONG = NULL,
    
    .SHORT = "h::a::b::c::d::e::",
    
    .FUNCTIONS = (option_function[])
    {
        OPTION_ADD_FUNCTION(mem, h),
        OPTION_ADD_FUNCTION(mem, a),
        OPTION_ADD_FUNCTION(mem, b),
        OPTION_ADD_FUNCTION(mem, c),
        OPTION_ADD_FUNCTION(mem, d),
        OPTION_ADD_FUNCTION(mem, e),
    }
};

#endif //AVIONICS_MEM_H
