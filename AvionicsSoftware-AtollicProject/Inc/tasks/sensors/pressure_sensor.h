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

#define TIMEOUT 100 // milliseconds

//Groups a time stamp with the reading.
typedef struct {
	bmp3_data_t data;
	uint32_t time_ticks; //time of sensor reading in ticks.
} pressure_sensor_data;

//Parameters for thread_pressure_sensor_start.
typedef struct{
	UART_HandleTypeDef * huart;
	QueueHandle_t	bmp388_queue;
	configData_t *flightCompConfig;
} thread_pressure_sensor_parameters;


void init_bmp(configData_t * configParams);
void calibrate_bmp(configData_t * configParams);

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
