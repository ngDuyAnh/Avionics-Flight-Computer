#include "tasks/command_line_interface/tools/configure.h"
#include "tasks/command_line_interface/details/internals.h"
#include <stdlib.h>
#include "external/sensors/bmp3_defs.h"
#include "external/sensors/bmi08x_defs.h"

OPTION_NEW_TOOL_IMPL(configure)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    int option_index = 0;
    optind = 0; /* resetting the optind */
    int temp_optind = optind;
    while (1)
    {
        int opt;
        (CONFIGURATIONS_ARG_OPTIONS.LONG == NULL)
        ?
        (opt = getopt(argc, argv, CONFIGURATIONS_ARG_OPTIONS.SHORT))
        :
        (opt = getopt_long(argc, argv, CONFIGURATIONS_ARG_OPTIONS.SHORT, CONFIGURATIONS_ARG_OPTIONS.LONG, &option_index));
        
        if(opt == -1)
            break;
        
        // saving optind
        temp_optind = optind;
        
        switch(opt)
        {
            COMMAND_CASE_FUNC(300, 'h', configure, h, optarg);
            COMMAND_CASE_FUNC(301, 'a', configure, a, optarg);
            COMMAND_CASE_FUNC(302, 'b', configure, b, optarg);
            COMMAND_CASE_FUNC(303, 'c', configure, c, optarg);
            COMMAND_CASE_FUNC(304, 'd', configure, d, optarg);
            COMMAND_CASE_FUNC(305, 'e', configure, e, optarg);
            COMMAND_CASE_FUNC(306, 'f', configure, f, optarg);
            COMMAND_CASE_FUNC(307, 'g', configure, g, optarg);
            COMMAND_CASE_FUNC(308, 'i', configure, i, optarg);
            COMMAND_CASE_FUNC(309, 'j', configure, j, optarg);
            COMMAND_CASE_FUNC(310, 'k', configure, k, optarg);
            COMMAND_CASE_FUNC(311, 'l', configure, l, optarg);
            COMMAND_CASE_FUNC(312, 'm', configure, m, optarg);
            COMMAND_CASE_FUNC(313, 'n', configure, n, optarg);
            
            // Add more commands
            ERROR_CASE;
            DEFAULT_CASE(optarg, __s_uart, __s_output);
        }
        
        // restoring optind
        optind = temp_optind;
    }
    
    return true;
}


OPTION_FUNCTION_IMPL(configure, h)
{
    uart_transmit_line(__s_uart, CONFIGURATION_USAGE);
    return true;
}

OPTION_FUNCTION_IMPL(configure, a)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    if(value > 0 && value <= 100)
    {
        sprintf(__s_output, "Setting data rate to %d Hz.\n", value);
        
        uart_transmit_line(__s_uart, __s_output);
        __application_configurations->values.data_rate = 1000 / value;
    }
    return true;
}

OPTION_FUNCTION_IMPL(configure, b)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 0:
            sprintf(__s_output, "Turning off flash recording.\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.flags &= ~(0x02);
            break;
        case 1:
            sprintf(__s_output, "Turning on flash recording.\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.flags |= (0x02);
            break;
    }
    return true;
}

OPTION_FUNCTION_IMPL(configure, c)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 0:
            sprintf(__s_output, "Setting accelerometer to no over-sampling .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_bw = BMI08X_ACCEL_BW_NORMAL;
            break;
        case 2:
            sprintf(__s_output, "Setting accelerometer to 2x over-sampling .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_bw = BMI08X_ACCEL_BW_OSR2;
            break;
        case 4:
            sprintf(__s_output, "Setting accelerometer to 4x over-sampling .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_bw = BMI08X_ACCEL_BW_OSR4;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, d)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 3:
            sprintf(__s_output, "Setting accelerometer range to %d g .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_range = BMI088_ACCEL_RANGE_3G;
            break;
        case 6:
            sprintf(__s_output, "Setting accelerometer range to %d g .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_range = BMI088_ACCEL_RANGE_6G;
            break;
        case 12:
            sprintf(__s_output, "Setting accelerometer range to %d g .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_range = BMI088_ACCEL_RANGE_12G;
            break;
        case 24:
            sprintf(__s_output, "Setting accelerometer range to %d g .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_range = BMI088_ACCEL_RANGE_24G;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, e)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 12:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_12_5_HZ;
            break;
        case 25:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_25_HZ;
            break;
        case 50:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_50_HZ;
            break;
        case 100:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_100_HZ;
            break;
        case 200:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_200_HZ;
            break;
        case 400:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_400_HZ;
            break;
        case 800:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_800_HZ;
            break;
        case 1600:
            sprintf(__s_output, "Setting accelerometer odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.ac_odr = BMI08X_ACCEL_ODR_1600_HZ;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, f)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 1:
            sprintf(__s_output, "Setting gyroscope to BW_32_ODR_100_HZ .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_32_ODR_100_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_32_ODR_100_HZ;
            break;
        case 2:
            sprintf(__s_output, "Setting gyroscope BW_64_ODR_200_HZ .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_64_ODR_200_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_64_ODR_200_HZ;
            break;
        case 3:
            sprintf(__s_output, "Setting gyroscope BW_12_ODR_100_HZ  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_12_ODR_100_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_12_ODR_100_HZ;
            break;
        case 4:
            sprintf(__s_output, "Setting gyroscope BW_23_ODR_200_HZ  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_23_ODR_200_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_23_ODR_200_HZ;
            break;
        case 5:
            sprintf(__s_output, "Setting gyroscope BW_47_ODR_400_HZ  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_47_ODR_400_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_47_ODR_400_HZ;
            break;
        case 6:
            sprintf(__s_output, "Setting gyroscope BW_116_ODR_1000_HZ  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_116_ODR_1000_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_116_ODR_1000_HZ;
            break;
        case 7:
            sprintf(__s_output, "Setting gyroscope BW_230_ODR_2000_HZ  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_230_ODR_2000_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_230_ODR_2000_HZ;
            break;
        case 8:
            sprintf(__s_output, "Setting gyroscope BW_532_ODR_2000_HZ  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_odr = BMI08X_GYRO_BW_532_ODR_2000_HZ;
            __application_configurations->values.gy_bw = BMI08X_GYRO_BW_532_ODR_2000_HZ;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, g)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 125:
            sprintf(__s_output, "Setting gyroscope range to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_range = BMI08X_GYRO_RANGE_125_DPS;
            break;
        case 250:
            sprintf(__s_output, "Setting gyroscope range to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_range = BMI08X_GYRO_RANGE_250_DPS;
            break;
        case 500:
            sprintf(__s_output, "Setting gyroscope range to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_range = BMI08X_GYRO_RANGE_500_DPS;
            break;
        case 1000:
            sprintf(__s_output, "Setting gyroscope range to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_range = BMI08X_GYRO_RANGE_1000_DPS;
            break;
        case 2000:
            sprintf(__s_output, "Setting gyroscope range to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.gy_range = BMI08X_GYRO_RANGE_2000_DPS;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, i)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 1:
            sprintf(__s_output, "Setting bmp odr to 1.5 Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.bmp_odr = BMP3_ODR_1_5_HZ;
            break;
        case 12:
            sprintf(__s_output, "Setting bmp odr to 12.5 Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.bmp_odr = BMP3_ODR_12_5_HZ;
            break;
        case 25:
            sprintf(__s_output, "Setting bmp odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.bmp_odr = BMP3_ODR_25_HZ;
            break;
        case 50:
            sprintf(__s_output, "Setting bmp odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.bmp_odr = BMP3_ODR_50_HZ;
            break;
        case 100:
            sprintf(__s_output, "Setting bmp odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.bmp_odr = BMP3_ODR_100_HZ;
            break;
        case 200:
            sprintf(__s_output, "Setting bmp odr to %d Hz .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.bmp_odr = BMP3_ODR_200_HZ;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, j)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 0:
            sprintf(__s_output, "Setting pressure oversampling 0x .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.pres_os = BMP3_NO_OVERSAMPLING;
            break;
        case 2:
            sprintf(__s_output, "Setting pressure oversampling 2x  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.pres_os = BMP3_OVERSAMPLING_2X;
            break;
        case 4:
            sprintf(__s_output, "Setting pressure oversampling 4x .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.pres_os = BMP3_OVERSAMPLING_4X;
            break;
        case 8:
            sprintf(__s_output, "Setting pressure oversampling to 8x Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.pres_os = BMP3_OVERSAMPLING_8X;
            break;
        case 16:
            sprintf(__s_output, "Setting pressure oversampling to 16x Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.pres_os = BMP3_OVERSAMPLING_16X;
            break;
        case 32:
            sprintf(__s_output, "Setting pressure oversampling to 32x Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.pres_os = BMP3_OVERSAMPLING_32X;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, k)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 0:
            sprintf(__s_output, "Setting temperature oversampling 0x .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_NO_OVERSAMPLING;
            break;
        case 2:
            sprintf(__s_output, "Setting temperature oversampling 2x  .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_OVERSAMPLING_2X;
            break;
        case 4:
            sprintf(__s_output, "Setting temperature oversampling 4x .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_OVERSAMPLING_4X;
            break;
        case 8:
            sprintf(__s_output, "Setting temperature oversampling to 8x Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_OVERSAMPLING_8X;
            break;
        case 16:
            sprintf(__s_output, "Setting temperature oversampling to 16x Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_OVERSAMPLING_16X;
            break;
        case 32:
            sprintf(__s_output, "Setting temperature oversampling to 32x Hz .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_OVERSAMPLING_32X;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, l)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 0:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to off .\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_DISABLE;
            break;
        case 1:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to %d .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_COEFF_1;
            break;
        case 3:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to %d .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_COEFF_3;
            break;
        case 7:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to %d .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_COEFF_7;
            break;
        case 15:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to %d .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_COEFF_15;
            break;
        case 31:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to %d .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_COEFF_31;
            break;
        case 63:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to %d .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_COEFF_63;
            break;
        case 127:
            sprintf(__s_output, "Setting bmp IIR filter coefficient to %d .\n", value);
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.temp_os = BMP3_IIR_FILTER_COEFF_127;
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, m)
{
    sprintf(__s_output, "The current settings (not in flash):");
    uart_transmit_line(__s_uart, __s_output);
    
    sprintf(__s_output, "ID: %d \tInitial Time To Wait: %i \r\n", __application_configurations->values.id, __application_configurations->values.initial_time_to_wait);
    uart_transmit_line(__s_uart, __s_output);
    
    sprintf(__s_output, "data rate: %d Hz \tSet to record: %d \r\n", 1000 / __application_configurations->values.data_rate,
            IS_RECORDING(__application_configurations->values.flags));
    uart_transmit_line(__s_uart, __s_output);
    
    sprintf(__s_output, "start of data: %i \tend of data: %i \r\n", __application_configurations->values.start_data_address, __application_configurations->values.end_data_address);
    uart_transmit_line(__s_uart, __s_output);
    
    sprintf(__s_output, "reference altitude: %i \t reference pressure: %i \r\n", (uint32_t) __application_configurations->values.ref_alt, (uint32_t) __application_configurations->values.ref_pres);
    uart_transmit_line(__s_uart, __s_output);
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, n)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    char val_str[10];
    strcpy(val_str, argv[1]);
    int value = atoi(val_str);
    switch(value)
    {
        case 0:
            sprintf(__s_output, "Setting to not in flight.\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.flags &= ~(0x1D);
            break;
        case 1:
            sprintf(__s_output, "Setting to in flight.\n");
            uart_transmit_line(__s_uart, __s_output);
            __application_configurations->values.flags |= (0x01);
            break;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(configure, z)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    char val_str[15];
    strcpy(val_str, argv[1]);
    
    int value = atoi(val_str);
    if(value > 0 && value <= 10000000)
    {
        sprintf(__s_output, "Setting initial time to wait to %d ms.\n", value);
        uart_transmit_line(__s_uart, __s_output);
        __application_configurations->values.initial_time_to_wait = value;
        
    }
    
    return true;
}
