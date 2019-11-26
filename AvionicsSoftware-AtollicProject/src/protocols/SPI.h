//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS Rocketry 2019
//
// Repository:
//  UMSATS>Avionics-2019
//
//
// File Description:
//  Header file for SPI.c.
//
// History
// 2019-02-06 by Joseph Howarth
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef SPI_H
#define SPI_H

#include <inttypes.h>

typedef void* SPI;
// Description:
//  This function initializes the SPI1 interface.
SPI spi1_init();


// Description:
//  This function initializes the SPI2 interface.
SPI spi2_init();


// Description:
//  This function initializes the SPI3 interface.
SPI spi3_init();

// Description:
//  This function reads one or more bytes over the SPI bus, by sending multiple address bytes
//  and then reading multiple bytes.
//
// Parameters:
//     hspi             A reference used to refer to the SPI interface.
//     addr_buffer      A pointer to the buffer holding address to read from.
//	   addr_buffer_size The number of bytes in the address/command.
//     rx_buffer        A pointer to where the received bytes should be stored
//     rx_buffer_size   The number of bytes being  received.
//     timeout          The timeout value in milliseconds.
void spi_receive(SPI hspi,uint8_t *addr_buffer,uint8_t addr_buffer_size,uint8_t *rx_buffer,uint16_t rx_buffer_size, uint32_t timeout);


// Description:
//  This function transfers one or more bytes over the SPI bus.
//  It firstly sends multiple register address bytes.
//
// Parameters:
//     hspi            	A reference used to refer to the SPI interface.
//     addr_buffer     	A pointer to the buffer holding the address to write to.
//	   addr_buffer_size	Number of bytes in the address/command.
//     tx_buffer       	A pointer to the bytes to send.
//     size            	The number of bytes being sent.
//     timeout         	The timeout value in milliseconds.
void spi_send(SPI hspi, uint8_t *reg_addr,uint8_t reg_addr_size, uint8_t *tx_buffer, uint16_t tx_buffer_size, uint32_t timeout);


// Description: DO NOT USE. Will be deleted in future versions of the code!
//  This function reads one or more bytes over the SPI bus, by sending the address
//  and then reading
//
// Parameters:
//     hspi            A reference used to refer to the SPI interface.
//     addr_buffer     A pointer to the address to read from.
//     rx_buffer       A pointer to where the received bytes should be stored
//     total_size      The number of bytes being sent and received. (# of bytes read + 1)
//     timeout         The timeout value in milliseconds.

void spi_read(SPI hspi,uint8_t *addr_buffer,uint8_t *rx_buffer,uint16_t total_size, uint32_t timeout);


// Description:  DO NOT USE! Will be deleted in future versions of the code!
//  This function transfers one or more bytes over the SPI bus.
//  It firstly sends the register address (hard coded to be a 1 byte address).
//
// Parameters:
//     hspi            A reference used to refer to the SPI interface.
//     addr_buffer     A pointer to the address to write to.
//     tx_buffer       A pointer to the bytes to send.
//     size            The number of bytes being sent.
//     timeout         The timeout value in milliseconds.
void spi_transmit(SPI hspi, uint8_t *reg_addr, uint8_t *tx_buffer,uint16_t total_size, uint32_t timeout);

#endif /* SPI_H_ */

