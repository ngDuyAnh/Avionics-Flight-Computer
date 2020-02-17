#ifndef STM32F4XX_HAL_UART_CLI_H
#define STM32F4XX_HAL_UART_CLI_H
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS 2018-2020
//
// Repository:
//  UMSATS>Avionics-2019
//
// File Description:
//  Header file for communicating with STM32 microchip via UART Serial Connection. Handles initialization and transmission/reception.
//
// History
// 2019-02-13 Eric Kapilik
// - Created.

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#define TIMEOUT_MAX 0xFFFF
#define BUFFER_SIZE 2048

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//	For UART port 2 only, this should only be run ONCE. (i.e. only one program should call it, once.)
//  Enable UART clock, initialize GPIO pins, and create UART structure required for communication.
//	Uses GPIO pins 2 & 3 of STM32F401RE
//
// NOTE: If you wish to use another UART port, please create another function to initialize that port.
//
// Parameters:
//  UART_HandleTypeDef Pointer (needed by communication functions)
//
// Returns:
//  VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
int UART_Port2_Init(void);


//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//	For UART port 6 only, this should only be run ONCE. (i.e. only one program should call it, once.)
//  Enable UART clock, initialize GPIO pins, and create UART structure required for communication.
//
// NOTE: If you wish to use another UART port, please create another function to initialize that port.
//
// Parameters:
//  UART_HandleTypeDef Pointer (needed by communication functions)
//
// Returns:
//  VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
int UART_Port6_Init(void);


//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Transmit message to UART port. Does not add new line to message.
//
// Parameters:
//  UART uart - UART port to uart_transmit to
//  char* message - the message you wish to send, ending string with null character ('\0')
//
// Returns:
//  VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
int uart2_transmit(const char * message);
int uart6_transmit(const char * message);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Transmit message to UART port. Adds new line characters to end of message.
//
// Parameters:
//  UART uart - UART port to uart_transmit to
//  char* message - the message you wish to send, ending string with null character ('\0')
//
// Returns:
//  VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
int uart2_transmit_line(const char * message);
int uart6_transmit_line(const char * message);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Transmit bytes to UART port.
//
// Parameters:
//  UART uart - UART port to uart_transmit to
//  uint8_t* bytes - A pointer to the bytes you want to send.
//
// Returns:
//  VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
int uart2_transmit_bytes(uint8_t * bytes, uint16_t numBytes);
int uart6_transmit_bytes(uint8_t * bytes, uint16_t numBytes);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//	BLOCKING FUNCTION
//  Receive message from UART port. Prints back what the user is typing so that they can see it. Their message ends when they press enter.
//
// Parameters:
//  UART uart - UART port to uart_transmit to
//
// Returns:
//  Pointer to character array containing user entered message
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
char* uart2_receive_command();
char* uart6_receive_command();

int uart2_receive(uint8_t * buf, size_t size);
int uart6_receive(uint8_t * buf, size_t size);

#endif //STM32F4XX_HAL_UART_CLI_H
