#ifndef AVIONICS_STM32_H
#define AVIONICS_STM32_H

#include <inttypes.h>

typedef enum
{
	STM32_OK,
	STM32_ERROR,
	STM32_BUSY,
	STM32_TIMEOUT,
	STM32_SYS_CLOCK_CONFIG_ERROR
} STM32Status;


STM32Status stm32_init(void);
void stm32_error_handler(void);
void stm32_delay(uint32_t ms);
void stm32_led_blink();


#endif //AVIONICS_STM32_H
