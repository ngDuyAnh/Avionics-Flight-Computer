#include "mem.h"
#include "tasks/command_line_interface/details/internals.h"
#include "../definitions.h"
#include "hardware_definitions.h"
#include "flash.h"
#include <stdlib.h>


CREATE_OPT_DEFAULT_FUNCTION(mem, default_behaviour)
{
    sprintf(__s_output, "Command [%s] not recognized.", arguments);
    uart6_transmit_line(__s_output);

    return true;
}

CREATE_OPT_ERROR_FUNCTION(mem, error_behaviour)
{
    sprintf(__s_output, "Invalid option [-%s]\n", arguments);
    uart6_transmit_line(__s_output);
}


OPTION_NEW_TOOL_IMPL(mem)
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
        (MEMORY_MENU_ARG_OPTIONS.LONG == NULL)
        ?
        (opt = getopt(argc, argv, MEMORY_MENU_ARG_OPTIONS.SHORT))
        :
        (opt = getopt_long(argc, argv, MEMORY_MENU_ARG_OPTIONS.SHORT, MEMORY_MENU_ARG_OPTIONS.LONG, &option_index));

        if(opt == -1)
            break;

        // saving optind
        temp_optind = optind;

        switch(opt)
        {
            OPT_CASE_FUNC(300, 'h', mem, h, optarg);
            OPT_CASE_FUNC(301, 'a', mem, a, optarg);
            OPT_CASE_FUNC(302, 'b', mem, b, optarg);
            OPT_CASE_FUNC(303, 'c', mem, c, optarg);
            OPT_CASE_FUNC(304, 'd', mem, d, optarg);
            OPT_CASE_FUNC(305, 'e', mem, e, optarg);
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

OPTION_FUNCTION_IMPL(mem, h)
{
    uart6_transmit_line(MEMORY_MENU_USAGE);
    return true;
}

OPTION_FUNCTION_IMPL(mem, a)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
    char val_str[10];
    
    // assuming that I will pass letter a and the values (e.g. -a 56)
    strcpy(val_str, argv[1]);
    
    uint32_t value = strtol(val_str, NULL, 16);
    
    if(value >= 0 && value <= FLASH_END_ADDRESS)
    {
        sprintf(__s_output, "Reading 256 bytes starting at address %i ...", value);
        uart6_transmit_line(__s_output);
        
        uint8_t data_rx[FLASH_PAGE_SIZE];
        FlashStatus stat;
        stat = flash_read(value, data_rx, FLASH_PAGE_SIZE);
        
        uint8_t busy = stat;
        
        while(FLASH_IS_DEVICE_BUSY(busy))
        {
            busy = flash_get_status_register();
            vTaskDelay(pdMS_TO_TICKS(1));
        }
        
        if(stat == FLASH_OK)
        {
            sprintf(__s_output, "Success:");
        }
        else
        {
            sprintf(__s_output, "Failed:");
            return false;
        }
        uart6_transmit_line(__s_output);
        
        for(size_t i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            if((i + 1) % 16 == 0)
            {
                sprintf(__s_output, "0x%02X ", data_rx[i]);
                uart6_transmit_line(__s_output);
            }else
            {
                sprintf(__s_output, "0x%02X ", data_rx[i]);
                uart6_transmit(__s_output);
            }
        }
        uart6_transmit_line("\r\n");
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(mem, b)
{
    uint32_t end_Address = flash_scan();
    sprintf(__s_output, "end address :%i \n", end_Address);
    uart6_transmit_line(__s_output);
    
    return true;
}

OPTION_FUNCTION_IMPL(mem, c)
{
    uint8_t dataRX[FLASH_PAGE_SIZE];
    
    sprintf(__s_output, "Erasing data section ...");
    uart6_transmit_line(__s_output);
    
    uint32_t address = FLASH_START_ADDRESS;
    FlashStatus stat = FLASH_ERROR;
    
    while(address <= FLASH_END_ADDRESS)
    {
        
        if(address > FLASH_PARAM_END_ADDRESS)
        {
            stat = flash_erase_sector(address);
            address += FLASH_SECTOR_SIZE;
        }else
        {
            stat = flash_erase_param_sector(address);
            address += FLASH_PARAM_SECTOR_SIZE;
        }
        //Wait for erase to finish
        while(FLASH_IS_DEVICE_BUSY(stat))
        {
            
            stat = flash_get_status_register();
            
            vTaskDelay(pdMS_TO_TICKS(1));
        }

//        HAL_GPIO_TogglePin(USR_LED_PORT,USR_LED_PIN);
        sprintf(__s_output, "Erasing sector %i ...", address);
        uart6_transmit_line(__s_output);
    }
	
	flash_read(FLASH_START_ADDRESS, dataRX, 256);
    uint16_t empty = 0xFFFF;
    int i;
    for(i = 0; i < 256; i++)
    {
        
        if(dataRX[i] != 0xFF)
        {
            empty--;
        }
    }
    
    if(empty == 0xFFFF)
    {
        
        uart6_transmit_line("Flash Erased Success!");
    }
    
    if(stat == FLASH_OK)
    {
        sprintf(__s_output, "Success:");
    }else
    {
        sprintf(__s_output, "Failed:");
        return false;
    }
    uart6_transmit_line(__s_output);
    
    return true;
}

OPTION_FUNCTION_IMPL(mem, d)
{
    // TODO: implement
    return false;
}

OPTION_FUNCTION_IMPL(mem, e)
{
    // TODO: implement
    return false;
}
