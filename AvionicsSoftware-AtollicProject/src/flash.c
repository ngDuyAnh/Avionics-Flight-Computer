/**
 * @file flash.c
 * @author UMSATS Rocketry Division. Joseph Howarth
 * @date 2019-03-29
 * @brief Source file for the flash memory interface.
 *
 * Here typically goes a more extensive explanation of what the header
 * defines. Doxygens tags are words preceeded by either a backslash @\
 * or by an at symbol @@.
 * @see https://github.com/UMSATS/Avionics-2019
 */

#include <stdint.h>
#include "stm32/STM32.h"

#include "FreeRTOS.h"
#include "portable.h"

#include "flash.h"
#include "hardware_definitions.h"
#include "protocols/SPI.h"


struct flash_t
{
    SPI spi_handle; /**< SPI handle. */
};

typedef struct flash_t* Flash;

/**
 * @brief
 * This function sets the write enable. This is needed before a
 * write status register, program or erase command.
 * @param p_flash Pointer to @c Flash structure
 * @return Will be FLASH_BUSY if there is another operation in progress, FLASH_OK otherwise.
 * @see https://github.com/UMSATS/Avionics-2019/
 */
FlashStatus enable_write(Flash flash)
{
    uint8_t status_reg = flash_get_status_register(flash);
    if(FLASH_IS_DEVICE_BUSY(status_reg)){
        return FLASH_BUSY;
    }
    else{
        uint8_t command = FLASH_ENABLE_WRITE_COMMAND;
        spi_transmit(flash->spi_handle, &command, NULL, 1, 10);
        return FLASH_OK;
    }
}

/**
 * @brief
 * This function is responsible to work like a generic interface to send any command to the flash driver
 * @param p_flash Pointer to @c Flash structure
 * @param address pointer to where in the flash memory you want to apply the operation to
 * @return Will be FLASH_BUSY if there is another operation in progress, FLASH_OK otherwise.
 * @note Any additional commands should always call this function. If this function does not satisfy the
 * needs later on when the interface is extended to potentially support more operations
 * a developer should modify this function to his needs to keep this function as a generic interface forever
 * @see https://github.com/UMSATS/Avionics-2019/
 */
FlashStatus execute_command(Flash flash, uint32_t address, uint8_t command)
{
    uint8_t status_reg = flash_get_status_register(flash);
    if(FLASH_IS_DEVICE_BUSY(status_reg)){
        return FLASH_BUSY;
    }
    else{
        enable_write(flash);
        if(command == FLASH_BULK_ERASE_COMMAND)
        {
            enable_write(flash);
            spi_send(flash->spi_handle, &command, 1, NULL, 0, 10);
            return FLASH_OK;
        }
        else
        {
            uint8_t command_address[] =
                {
                    (command),
                    (address & (FLASH_HIGH_BYTE_MASK_24B)) >> 16,
                    (address & (FLASH_MID_BYTE_MASK_24B))  >> 8 ,
                    (address & (FLASH_LOW_BYTE_MASK_24B))
                };

            spi_send(flash->spi_handle, command_address, 4, NULL, 0, 10);
            return FLASH_OK;
        };
    }
}



uint8_t flash_get_status_register(Flash p_flash)
{
    uint8_t command = FLASH_GET_STATUS_REG_COMMAND;
    uint8_t status_reg;
    spi_receive(p_flash->spi_handle, &command, 1, &status_reg, 1, 10);
    return status_reg;
}


FlashStatus flash_erase_sector(Flash p_flash, uint32_t address)
{
    return execute_command(p_flash, address, FLASH_ERASE_SEC_COMMAND);
}

FlashStatus flash_erase_param_sector(Flash p_flash, uint32_t address)
{
    return execute_command(p_flash, address, FLASH_ERASE_PARAM_SEC_COMMAND);
}

FlashStatus flash_write(Flash p_flash, uint32_t address, uint8_t *data_buffer, uint16_t num_bytes)
{
    return execute_command(p_flash, address, FLASH_PP_COMMAND);
}

FlashStatus flash_read(Flash p_flash, uint32_t address, uint8_t *data_buffer, uint16_t num_bytes)
{
    return execute_command(p_flash, address, FLASH_READ_COMMAND);
}

FlashStatus flash_erase_device(Flash flash)
{
    return execute_command(flash, 0, FLASH_BULK_ERASE_COMMAND);
}

FlashStatus flash_check_id(Flash p_flash)
{
    uint8_t command = FLASH_READ_ID_COMMAND;
    uint8_t id[3] = {0, 0, 0};

    spi_receive(p_flash->spi_handle, (uint8_t *) &command, 1, id, 3, 10);
    if((id[0] == FLASH_MANUFACTURER_ID) && (id[1] == FLASH_DEVICE_ID_MSB) && (id[2] == FLASH_DEVICE_ID_LSB)){
        return FLASH_OK;
    }

    return FLASH_ERROR;
}


Flash flash_initialize()
{
    Flash flash = (Flash)pvPortMalloc(sizeof(struct flash_t));
    if(flash == NULL)
    {
        return NULL;
    }

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    //This configures the write protect pin(Active Low).
    GPIO_InitStruct.Pin = FLASH_WP_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct.Alternate = 0;

    HAL_GPIO_Init(FLASH_WP_PORT, &GPIO_InitStruct);

    GPIO_InitTypeDef GPIO_InitStruct2 = {0};
    //This configures the hold pin(Active Low).
    GPIO_InitStruct2.Pin = FLASH_HOLD_PIN;
    GPIO_InitStruct2.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct2.Pull = GPIO_NOPULL;
    GPIO_InitStruct2.Speed = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_InitStruct2.Alternate = 0;

    HAL_GPIO_Init(FLASH_HOLD_PORT, &GPIO_InitStruct2);

    HAL_GPIO_WritePin(FLASH_WP_PORT, FLASH_WP_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(FLASH_HOLD_PORT, FLASH_HOLD_PIN, GPIO_PIN_SET);
    //Set up the SPI interface
    spi1_init(&flash->spi_handle);

    HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET);
    
    if(FLASH_ERROR == flash_check_id(flash))
    {
        return NULL;
    }
    
    return flash;
}

size_t flash_scan(Flash p_flash)
{
    size_t result = 0;
    uint8_t dataRX[256];
    size_t i;
    int j;
    i = FLASH_START_ADDRESS;
    while(i < FLASH_SIZE_BYTES)
    {
        FlashStatus status;
        for(j = 0; j < 256; j++){
            dataRX[j] = 0;
        }

        status = flash_read(p_flash, i, dataRX, 256);
        uint16_t empty = 0xFFFF;
        for(j = 0; j < 256; j++)
        {
            if(dataRX[j] != 0xFF){
                empty--;
            }
        }

        if(empty == 0xFFFF)
        {
            result = i;
            break;
        }

        i = i + 256;
    }

    if(result == 0) result = FLASH_SIZE_BYTES;
    return result;
}
