#include "read.h"
#include "tasks/command_line_interface/details/internals.h"
#include "hardware_definitions.h"
#include "flash.h"


OPTION_NEW_TOOL_IMPL(read)
{
    uart_transmit_line(__s_uart,
        "Data transfer will start in 20 seconds. The LED will turn off when the transfer is complete.");
    
    uint8_t buffer[256 * 5]; //Read 5 pages from flash at a time;
    
    uint32_t bytesRead = 0;
    uint32_t currentAddress = FLASH_START_ADDRESS;
    
    vTaskDelay(pdMS_TO_TICKS(1000 * 10)); //Delay 10 seconds
    
    uint32_t endAddress = flash_scan(__flash);
    while(bytesRead < endAddress)
    {
		flash_read(__flash, currentAddress, buffer, 256 * 5);
        
        uint16_t empty = 0;
        for(uint16_t i = 0; i < 256 * 5; i++)
        {
            if(buffer[i] == 0xFF)
            {
                empty += 1;
            }
        }
        
        if(empty == (256 * 5))
        {
            break;
        }
        uart_transmit_bytes(__s_uart, buffer, 256 * 5);
        
        currentAddress += (256 * 5);
        currentAddress = currentAddress % FLASH_SIZE_BYTES;
        
        bytesRead += 256 * 5;
        vTaskDelay(1);
    }
    
    return true;
}
