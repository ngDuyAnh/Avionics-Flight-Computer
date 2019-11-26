#ifndef AVIONICS_CONFIGURE_H
#define AVIONICS_CONFIGURE_H

#include "../definitions.h"

DECL_NEW_USAGE(CONFIGURATION) = "Commands:\r\n"
                                "\t[h] - displays the help menu and more commands\r\n"
                                "\t[a] - Set data rate Hz(0-100)\r\n"
                                "\t[b] - set if recording to flash (1/0)\r\n"
                                "\t[c] - set accelerometer bandwidth (0,2,4)\r\n"
                                "\t[d] - set accelerometer range (3,6,12,24)\r\n"
                                "\t[e] - set accelerometer odr (12,25,50,100,200,400,800,1600)\r\n"
                                "\t[f] - set gyro bandwidth and odr (32_100,64_200,12_100,23_200,47_400,116_1000,230_2000,532_2000)\r\n\t(Enter the number (1-8) for the option to select)\r\n"
                                "\t[g] - set gyro range (125,250,500,1000,2000)\r\n"
                                "\t[i] - set BMP388 odr (1,12,25,50,100,200) \r\n"
                                "\t[j] - set pressure oversampling (0,2,4,8,16,32) \r\n"
                                "\t[k] - set temperature oversampling (0,2,4,8,16,32) \r\n"
                                "\t[l] - set BMP388 IIR filter coefficient (0,1,3,7,15,31,63,127) \r\n"
                                "\t[m] - Read the current settings\r\n"
                                "\t[n] - Set if in flight (1/0)\r\n"
                                "\t[z] - Set the initial time to wait (0-10000000)\r\n";


OPTION_NEW_TOOL_DECL(configure);
OPTION_FUNCTION_DECL(configure, h);
OPTION_FUNCTION_DECL(configure, a);
OPTION_FUNCTION_DECL(configure, b);
OPTION_FUNCTION_DECL(configure, c);
OPTION_FUNCTION_DECL(configure, d);
OPTION_FUNCTION_DECL(configure, e);
OPTION_FUNCTION_DECL(configure, f);
OPTION_FUNCTION_DECL(configure, g);
OPTION_FUNCTION_DECL(configure, i);
OPTION_FUNCTION_DECL(configure, j);
OPTION_FUNCTION_DECL(configure, k);
OPTION_FUNCTION_DECL(configure, l);
OPTION_FUNCTION_DECL(configure, m);
OPTION_FUNCTION_DECL(configure, n);
OPTION_FUNCTION_DECL(configure, z);


DECL_NEW_ARG_OPTIONS_FOR(CONFIGURATIONS) =
{
    .LONG = NULL,
    
    .SHORT = "a:b:c:d:e:f:g:i:j:k:l:n:z:mh::",
    
    .FUNCTIONS = (option_function[])
    {
        OPTION_ADD_FUNCTION(configure, h),
        OPTION_ADD_FUNCTION(configure, a),
        OPTION_ADD_FUNCTION(configure, b),
        OPTION_ADD_FUNCTION(configure, c),
        OPTION_ADD_FUNCTION(configure, d),
        OPTION_ADD_FUNCTION(configure, e),
        OPTION_ADD_FUNCTION(configure, f),
        OPTION_ADD_FUNCTION(configure, g),
        OPTION_ADD_FUNCTION(configure, i),
        OPTION_ADD_FUNCTION(configure, j),
        OPTION_ADD_FUNCTION(configure, k),
        OPTION_ADD_FUNCTION(configure, l),
        OPTION_ADD_FUNCTION(configure, m),
        OPTION_ADD_FUNCTION(configure, n),
        OPTION_ADD_FUNCTION(configure, z),
    }
};


#endif //AVIONICS_CONFIGURE_H
