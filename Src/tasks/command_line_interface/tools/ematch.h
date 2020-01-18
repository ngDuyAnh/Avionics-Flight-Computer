#ifndef AVIONICS_EMATCH_H
#define AVIONICS_EMATCH_H

#include "../definitions.h"

OPTION_NEW_TOOL_DECL(ematch);
OPTION_FUNCTION_DECL(ematch, h);
OPTION_FUNCTION_DECL(ematch, a);
OPTION_FUNCTION_DECL(ematch, b);
OPTION_FUNCTION_DECL(ematch, c);
OPTION_FUNCTION_DECL(ematch, d);
OPTION_FUNCTION_DECL(ematch, e);
OPTION_FUNCTION_DECL(ematch, f);
OPTION_FUNCTION_DECL(ematch, g);
OPTION_FUNCTION_DECL(ematch, i);
OPTION_FUNCTION_DECL(ematch, j);

DECL_NEW_USAGE(EMATCH) = "E-Matches:\r\n"
                         "\t[h] - displays the help menu and more commands\r\n"
                         "\t[a] - Check continuity Drogue\r\n"
                         "\t[b] - Check continuity Main\r\n"
                         "\t[c] - Check overcurrent Drogue\r\n"
                         "\t[d] - Check overcurrent Main\r\n"
                         "\t[e] - Enable Drogue\r\n"
                         "\t[f] - Enable Main\r\n"
                         "\t[g] - Fire Drogue (delayed)\r\n"
                         "\t[i] - Fire Main   (delayed)\r\n"
                         "\t[j] - Set delay_ms (5-60)\r\n ";


DECL_NEW_ARG_OPTIONS_FOR(EMATCH) =
{
    .LONG = NULL,
    
    .SHORT = "h::a::b::c::d::e::f::g::i::j::",
    
    .FUNCTIONS = (option_function[])
    {
        OPTION_ADD_FUNCTION(ematch, h),
        OPTION_ADD_FUNCTION(ematch, a),
        OPTION_ADD_FUNCTION(ematch, b),
        OPTION_ADD_FUNCTION(ematch, c),
        OPTION_ADD_FUNCTION(ematch, d),
        OPTION_ADD_FUNCTION(ematch, e),
        OPTION_ADD_FUNCTION(ematch, f),
        OPTION_ADD_FUNCTION(ematch, g),
        OPTION_ADD_FUNCTION(ematch, i),
        OPTION_ADD_FUNCTION(ematch, j),
    }
};


#endif //AVIONICS_EMATCH_H
