#include "mem.h"
#include "tasks/command_line_interface/details/internals.h"
#include "../definitions.h"
#include "hardware_definitions.h"
#include "flash.h"
#include <stdlib.h>


CREATE_OPT_DEFAULT_FUNCTION(mem, default_behaviour)
{
    sprintf(__s_output, "Command [%s] not recognized.", arguments);
    uart_transmit_line(__s_uart, arguments);
    
    return true;
}

CREATE_OPT_ERROR_FUNCTION(mem, error_behaviour)
{
    // TODO: do something
}


OPTION_NEW_TOOL_IMPL(mem)
{
    EXPAND_ARGUMENTS_STRING(ARGUMENTS_STRING);
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
            
            OPT_ERROR_FUNC  (mem,  error_behaviour,   ARGUMENTS_STRING);
            OPT_DEFAULT_FUNC(mem, default_behaviour,  ARGUMENTS_STRING);
        }
        
        // restoring optind
        optind = temp_optind;
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(mem, h)
{
    uart_transmit_line(__s_uart, MEMORY_MENU_USAGE);
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
        uart_transmit_line(__s_uart, __s_output);
        
        uint8_t data_rx[FLASH_PAGE_SIZE];
        FlashStatus stat;
        stat = flash_read_page(__flash, value, data_rx, FLASH_PAGE_SIZE);
        
        uint8_t busy = stat;
        
        while(FLASH_IS_DEVICE_BUSY(busy))
        {
            busy = flash_get_status_register(__flash);
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
        uart_transmit_line(__s_uart, __s_output);
        
        for(size_t i = 0; i < FLASH_PAGE_SIZE; i++)
        {
            if((i + 1) % 16 == 0)
            {
                sprintf(__s_output, "0x%02X ", data_rx[i]);
                uart_transmit_line(__s_uart, __s_output);
            }else
            {
                sprintf(__s_output, "0x%02X ", data_rx[i]);
                uart_transmit(__s_uart, __s_output);
            }
        }
        uart_transmit_line(__s_uart, "\r\n");
    }
    
    return true;
}

OPTION_FUNCTION_IMPL(mem, b)
{
    uint32_t end_Address = flash_scan(__flash);
    sprintf(__s_output, "end address :%i \n", end_Address);
    uart_transmit_line(__s_uart, __s_output);
    
    return true;
}

OPTION_FUNCTION_IMPL(mem, c)
{
    uint8_t dataRX[FLASH_PAGE_SIZE];
    
    sprintf(__s_output, "Erasing data section ...");
    uart_transmit_line(__s_uart, __s_output);
    
    uint32_t address = FLASH_START_ADDRESS;
    FlashStatus stat = FLASH_ERROR;
    
    while(address <= FLASH_END_ADDRESS)
    {
        
        if(address > FLASH_PARAM_END_ADDRESS)
        {
            stat = flash_erase_sector(__flash, address);
            address += FLASH_SECTOR_SIZE;
        }else
        {
            stat = flash_erase_param_sector(__flash, address);
            address += FLASH_PARAM_SECTOR_SIZE;
        }
        //Wait for erase to finish
        while(FLASH_IS_DEVICE_BUSY(stat))
        {
            
            stat = flash_get_status_register(__flash);
            
            vTaskDelay(pdMS_TO_TICKS(1));
        }

//        HAL_GPIO_TogglePin(USR_LED_PORT,USR_LED_PIN);
        sprintf(__s_output, "Erasing sector %i ...", address);
        uart_transmit_line(__s_uart, __s_output);
    }
    
    flash_read_page(__flash, FLASH_START_ADDRESS, dataRX, 256);
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
        
        uart_transmit_line(__s_uart, "Flash Erased Success!");
    }
    
    if(stat == FLASH_OK)
    {
        sprintf(__s_output, "Success:");
    }else
    {
        sprintf(__s_output, "Failed:");
        return false;
    }
    uart_transmit_line(__s_uart, __s_output);
    
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
