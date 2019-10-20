#ifndef PRESSURE_SENSOR_BMP3_H
#define PRESSURE_SENSOR_BMP3_H
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// pressure_sensor_bmp3.h
// UMSATS 2018-2020
//
// Repository:
//  UMSATS > Avionics 2019
//
// File Description:
//  Control and usage of BMP3 sensor inside of RTOS task.
//
// History
// 2019-03-04 Eric Kapilik
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#include <inttypes.h>
#include "forward_declarations.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "bmp3.h"

#define	PRES_LENGTH	3		//Length of a pressure measurement in bytes.
#define	TEMP_LENGTH	3		//Length of a temperature measurement in bytes.
#define ALT_LENGTH  4
#define TIMEOUT 100 // milliseconds

//Groups a time stamp with the reading.
typedef struct pressure_sensor_data{
	bmp3_data_t data;
	uint32_t time_ticks; //time of sensor reading in ticks.
} pressure_sensor_data;

typedef union {
	float float_val;
	uint32_t byte_val;
} altitude_data;

//Parameters for thread_pressure_sensor_start.
typedef struct{
	UART_HandleTypeDef * huart;
	QueueHandle_t	bmp388_queue;
	configuration_data_t *flightCompConfig;
} thread_pressure_sensor_parameters;


void init_bmp(configuration_data_t * configParams);
void calibrate_bmp(configuration_data_t * configParams);
altitude_data calculate_altitude(float pressure, float temperature, float ref_pres, float ref_alt);

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Task for testing the BMP3 sensor.
//    - initializes sensor
//    - read data & print to UART screen cycle
//
// Returns:
//  VOID
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
void thread_pressure_sensor_start(void *pvParameters);



#endif // PRESSURE_SENSOR_BMP3_H
