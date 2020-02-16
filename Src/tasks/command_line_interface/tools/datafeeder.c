//
// Created by vasil on 1/17/2020.
//

#include "datafeeder.h"
#include <stdlib.h>
#include <tasks/sensors/imu_sensor.h>
#include <tasks/sensors/pressure_sensor.h>

#include "utilities/common.h"
#include "tasks/command_line_interface/details/internals.h"
#include "external/sensors/bmp3.h"


static const char * END_SEQEUNCE = "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";

OPTION_NEW_TOOL_IMPL(datafeeder)
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


        (DATAFEEDER_ARG_OPTIONS.LONG == NULL)
        ?
        (opt = getopt(argc, argv, DATAFEEDER_ARG_OPTIONS.SHORT))
        :
        (opt = getopt_long(argc, argv, DATAFEEDER_ARG_OPTIONS.SHORT, DATAFEEDER_ARG_OPTIONS.LONG, &option_index));

        if(opt == -1)
            break;

        // saving optind
        temp_optind = optind;

        switch(opt)
        {
            OPT_CASE_FUNC(300, 'h', datafeeder, h, optarg);
            OPT_CASE_FUNC(301, 's', datafeeder, s, optarg);

            // Add more commands
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


OPTION_FUNCTION_IMPL(datafeeder, h) {
    PRINT("%s\n", DATAFEEDER_USAGE);
    return true;
}


OPTION_FUNCTION_IMPL(datafeeder, s)
{
    PRINT("OK", NULL);
    while(1)
    {
        uint8_t buf[36];
        if(uart_receive(__s_uart, buf, 36))
        {
            if(memcmp(buf, END_SEQEUNCE, 36) == 0) {
                PRINT("OK", NULL);
                break;
            }

            uint32_t timestamp      = to_uint32_t(&buf[0]);
            int32_t accel[3]        = {to_int32_t(&buf[4]),  to_int32_t(&buf[8]),  to_int32_t(&buf[12])};
            int32_t gyro [3]        = {to_int32_t(&buf[16]), to_int32_t(&buf[20]), to_int32_t(&buf[24])};
            uint32_t pressure       = to_uint32_t(&buf[28]);
            uint32_t temperature    = to_uint32_t(&buf[32]);

            // process IMU data
//            imu_sensor_data imu = {.time_ticks = timestamp, .gyro_x = gyro[1], .gyro_y = gyro[2], .gyro_z = gyro[3], .acc_x = accel[1], .acc_y = accel[2], .acc_z = accel[3]};
//            imu_add_measurement(&imu);

            // process pressure sensor data
            pressure_sensor_data pressureSensorData = {.time_ticks = timestamp, .pressure = pressure, .temperature = temperature};
            pressure_sensor_add_measurement(&pressureSensorData);
        }
    };

    return true;
}



