//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS 2018-2020
//
// Repository:
//  UMSATS/Avionics-2019
//
// File Description:
//  Source file for the data logging module.
//
// History
// 2019-04-10 by Joseph Howarth
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "tasks/data_recorder.h"
#include "cmsis_os.h"
#include "flash.h"                    //For flash memory functions
#include "tasks/sensors/pressure_sensor.h"    //For bmp reading struct
#include "stm32f4xx_hal_uart_io.h"
#include "tasks/sensors/imu_sensor.h"                //For imu_reading struct
#include "buzzer.h"
#include "recovery.h"
#include "altimeter.h"
#include "configuration.h"
#include "utilities/common.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// DEFINITIONS AND MACROS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// ENUMS AND ENUM TYPEDEFS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// STRUCTS AND STRUCT TYPEDEFS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// TYPEDEFS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//Checks if a measurement is empty. returns 0 if it is empty.
uint8_t isMeasurementEmpty(Measurement_t *measurement)
{
	uint8_t result = 0;
	for(size_t i = 0; i < sizeof(Measurement_t); i++)
	{
		if(measurement->data[i] != 0)
		{
			result++;
		}
	}
	return result;
}
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
struct buffer_container
{
	int d;
};

void check_recovery_circuit(configData_t * configParams)
{
	recoverySelect_t event_d = DROGUE;
	continuityStatus_t cont_d = check_continuity(event_d);
	recoverySelect_t event_m = MAIN;
	continuityStatus_t cont_m = check_continuity(event_m);
	
	while(cont_m == OPEN_CIRCUIT || cont_d == OPEN_CIRCUIT)
	{
		
		cont_m = check_continuity(event_m);
		cont_d = check_continuity(event_d);
	}
	configParams->values.state = STATE_LAUNCHPAD_ARMED;
	write_config(configParams);
}

void process_imu_data(imu_data_struct imu_reading, uint8_t* dest, uint32_t timestamp)
{
	// Make sure time doesn't overwrite type and event bits.
	uint32_t header = (ACC_TYPE | GYRO_TYPE) + (timestamp & 0x0FFF);
	write_24(header,                  &dest[0]);
	write_16(imu_reading.data_acc.x,  &dest[3]);
	write_16(imu_reading.data_acc.y,  &dest[5]);
	write_16(imu_reading.data_acc.z,  &dest[7]);
	write_16(imu_reading.data_gyro.x, &dest[9]);
	write_16(imu_reading.data_gyro.y, &dest[11]);
	write_16(imu_reading.data_gyro.z, &dest[13]);
}

float process_bmp_data_and_return_altitude(pressure_sensor_data bmp_reading, uint8_t * dest, float ref_pres, float ref_alt)
{
	//Update the header bytes.
	uint32_t header = (dest[0] << 16) + (dest[1] << 8) + dest[2];
	header |= PRES_TYPE | TEMP_TYPE;
	alt_value altitude = altitude_approx((float) bmp_reading.data.pressure, (float) bmp_reading.data.temperature, ref_pres, ref_alt);
	
	write_24(header, &dest[0]);
	write_24(bmp_reading.data.pressure,    &dest[15]);
	write_24(bmp_reading.data.temperature, &dest[18]);
	write_32(altitude.byte_val,            &dest[21]);

	return altitude.float_val;
}


void thread_data_recorder_start(void *params)
{
	
	thread_data_recorder_parameters *logStruct = (thread_data_recorder_parameters *) params;
	Flash *flash_ptr = logStruct->flash_ptr;
	UART_HandleTypeDef *huart = logStruct->uart;
	configData_t *configParams = logStruct->flightCompConfig;
	TaskHandle_t *timerTask_h = logStruct->timerTask_h;
	uint32_t flash_address = FLASH_START_ADDRESS;
	
	if(IS_IN_FLIGHT(configParams->values.flags)){
		flash_address = configParams->values.end_data_address;
	}

	int32_t acc_z_filtered;
	uint8_t acc_z_filter_index = 0;
	float total_filtered_altitude = 0;
	uint8_t alt_filter_count = 0;
	uint8_t running = 1;
	
	uint8_t data_bufferA[DATA_BUFFER_SIZE];        // This stores the data until we have enough to write to flash.
	uint8_t data_bufferB[DATA_BUFFER_SIZE];        // This stores the data until we have enough to write to flash.
	uint8_t launchpadBuffer[DATA_BUFFER_SIZE * 25];
	uint8_t is_there_data;                // Used to keep track of if the current measurement has data.
	uint8_t measurement_length = 0;
	
	uint16_t buffer_index_curr = 0;                    //The current index in the buffer.
	uint16_t ring_buff_size = DATA_BUFFER_SIZE * 25;
	
	Measurement_t measurement;
	BufferSelection_t buffer_selection = BUFFER_A;
	
	
	HAL_GPIO_WritePin(USR_LED_PORT, USR_LED_PIN, GPIO_PIN_RESET);
	
	//Make sure the measurement starts empty.
	int i;
	for(i = 0; i < sizeof(Measurement_t); i++){
		measurement.data[i] = 0;
	}
	
	imu_data_struct imu_reading;
	pressure_sensor_data bmp_reading;
	
	
	
	alt_value altitude;
	alt_value alt_prev;
	alt_prev.float_val = 0;
	uint8_t alt_count = 0;
	uint8_t alt_main_count = 0;
	uint16_t apogee_holdout_count = 0;
	uint8_t landed_counter = 0;
	
	if(!IS_IN_FLIGHT(configParams->values.flags)){
		check_recovery_circuit(configParams);
	}
	buzz(250); // CHANGE TO 2 SECONDS!!!!!!!
	
	uint32_t start_time = xTaskGetTickCount();
	while(1)
	{
		
		measurement_length = 0;
		
		/* IMU READING*******************************************************************************************************************************/
		
		//Try and get data from the IMU queue. Block for up to a quarter of the time between the fastest measurement.
		if(pdTRUE == xQueueReceive(logStruct->IMU_data_queue, &imu_reading, configParams->values.data_rate / 4))
		{
			//Check if the current measurement has data.
			if(!isMeasurementEmpty(&measurement))
			{
				uint16_t timestamp = imu_reading.time_ticks - start_time;
				process_imu_data(imu_reading, &measurement.data[0], timestamp);
				measurement_length = ACC_LENGTH + GYRO_LENGTH;
				start_time = imu_reading.time_ticks;
			}
			
			HAL_GPIO_TogglePin(USR_LED_PORT, USR_LED_PIN);
		}else
		{
			clear_buffer(measurement.data, sizeof(Measurement_t));
			continue;
		}
		
		/* BMP READING*******************************************************************************************************************************/
		//Try and get data from the BMP queue. Block for up to a quarter of the time between the fastest measurement.
		if(pdTRUE == xQueueReceive(logStruct->PRES_data_queue, &bmp_reading, configParams->values.data_rate / 4))
		{
			if(isMeasurementEmpty(&measurement))
			{
				// TODO: this does not make sense, how can we have imu
				//       readings and entering here if Measurement is Empty
				//We already have a imu reading.
				//Update the header bytes.
				float approx_altitude = process_bmp_data_and_return_altitude(bmp_reading, &measurement.data[0],
														 configParams->values.ref_pres, configParams->values.ref_alt);
				
				
				total_filtered_altitude += (approx_altitude - total_filtered_altitude) * 0.2;
				measurement_length += (PRES_LENGTH + TEMP_LENGTH + ALT_LENGTH);
				// TODO: why we are not updating start_time here like in the IMU clause?
			}
		}else
		{
			clear_buffer(measurement.data, sizeof(Measurement_t));
			continue;
		}
		
		
		
		if(configParams->values.state == STATE_LAUNCHPAD_ARMED && imu_reading.data_acc.x > 10892)
		{
			buzz(250);
			vTaskResume(*timerTask_h); //start fixed timers.
			configParams->values.state = STATE_IN_FLIGHT_PRE_APOGEE;
			configParams->values.flags = configParams->values.flags | 0x04;
			//Record the launch event.
			uint32_t header = (measurement.data[0] << 16) + (measurement.data[1] << 8) + measurement.data[2];
			header |= LAUNCH_DETECT;
			configParams->values.flags = configParams->values.flags | 0x01;
			write_config(configParams);
			write_24(header, &measurement.data[0]);
			
			
			uint16_t buff_end = (ring_buff_size);
			for(uint8_t j = 0; j < 25; j++)
			{
				//need to copy last portion into the bufferA. Make sure to start from right place, which wont be the next spot.
				
				if((buffer_index_curr + 256) < buff_end)
				{
					FlashStatus stat_f2 = flash_program_page(flash_ptr, flash_address,
															 &launchpadBuffer[buffer_index_curr], DATA_BUFFER_SIZE);
					while(FLASH_IS_DEVICE_BUSY(stat_f2))
					{
						stat_f2 = flash_get_status_register(flash_ptr);
						vTaskDelay(1);
					}
					buffer_index_curr += 256;
				}else
				{
					
					uint8_t buff_temp[256];
					memcpy(&buff_temp, &launchpadBuffer[buffer_index_curr], buff_end - buffer_index_curr);
					memcpy(&buff_temp[buff_end - buffer_index_curr], &launchpadBuffer,
						   DATA_BUFFER_SIZE - (buff_end - buffer_index_curr));
					
					FlashStatus stat_f2 = flash_program_page(flash_ptr, flash_address, buff_temp, DATA_BUFFER_SIZE);
					while(FLASH_IS_DEVICE_BUSY(stat_f2))
					{
						stat_f2 = flash_get_status_register(flash_ptr);
						vTaskDelay(1);
					}
					buffer_index_curr = DATA_BUFFER_SIZE - (buff_end - buffer_index_curr);
				}
				
				flash_address += DATA_BUFFER_SIZE;
			}
			buffer_index_curr = 0;
		}
		
		//Check if the rocket has landed.
		if(configParams->values.state == STATE_IN_FLIGHT_POST_MAIN)
		{
			
			if(alt_count > 0)
			{
				
				//If altitude is within a 1m range for 20 samples
				if(altitude.float_val > (alt_prev.float_val - 1.0) && altitude.float_val < (alt_prev.float_val + 1.0))
				{
					alt_count++;
					if(alt_count > 245)
					{
						alt_count = 201;
					}
				}else
				{
					alt_count = 0;
				}
				
			}else
			{
				
				alt_prev.float_val = altitude.float_val;
				alt_count++;
				if(alt_count > 245)
				{
					alt_count = 201;
				}
			}
			
			if((pow(imu_reading.data_gyro.x, 2) + pow(imu_reading.data_gyro.y, 2) + pow(imu_reading.data_gyro.z, 2)) <
			   63075)
			{
				//If the gyro readings are all less than ~4.4 deg/sec and the altitude is not changing then the rocket has probably landed.
				
				if(alt_count > 200)
				{
					
					configParams->values.state = STATE_LANDED;
					configParams->values.flags = configParams->values.flags & ~(0x01);
					write_config(configParams);
					uint32_t header = (measurement.data[0] << 16) + (measurement.data[1] << 8) + measurement.data[2];
					header |= LAND_DETECT;
					
					measurement.data[0] = (header >> 16) & 0xFF;
					measurement.data[1] = (header >> 8) & 0xFF;
					measurement.data[2] = (header) & 0xFF;
					
					//Put everything into low power mode.
					running = 0;
					
				}
			}
		}
		
		
		//Check if the altitude is below 1500ft, after the drogue has been deployed.
		if(configParams->values.state == STATE_IN_FLIGHT_POST_APOGEE)
		{
			
			if(total_filtered_altitude < 375.0)
			{
				//375m ==  1230 ft
				alt_main_count++;
			}else
			{
				alt_main_count = 0;
			}
			if(alt_main_count > 5)
			{
				//deploy main
				buzz(250);
				recoverySelect_t event = MAIN;
				enable_mosfet(event);
				activate_mosfet(event);
				continuityStatus_t cont = check_continuity(event);
				uint32_t header = (measurement.data[0] << 16) + (measurement.data[1] << 8) + measurement.data[2];
				header |= MAIN_DETECT;
				
				measurement.data[0] = (header >> 16) & 0xFF;
				measurement.data[1] = (header >> 8) & 0xFF;
				measurement.data[2] = (header) & 0xFF;
				
				if(cont == OPEN_CIRCUIT)
				{
					
					configParams->values.state = STATE_IN_FLIGHT_POST_MAIN;
					configParams->values.flags = configParams->values.flags | 0x10;
					write_config(configParams);
					uint32_t header = (measurement.data[0] << 16) + (measurement.data[1] << 8) + measurement.data[2];
					header |= MAIN_DEPLOY;
					measurement.data[0] = (header >> 16) & 0xFF;
					measurement.data[1] = (header >> 8) & 0xFF;
					measurement.data[2] = (header) & 0xFF;
					
				}
			}
		}
		
		//check if rocket has reached apogee.
		if(configParams->values.state == STATE_IN_FLIGHT_PRE_APOGEE)
		{
			
			apogee_holdout_count++;
			if(apogee_holdout_count > (20 * 15))
			{
				
				uint64_t acc_mag =
					pow(imu_reading.data_acc.x, 2) + pow(imu_reading.data_acc.y, 2) + pow(imu_reading.data_acc.z, 2);
				if(acc_mag < 1 && total_filtered_altitude > 9000.0)
				{
					//5565132 = 3 * 1362^2 (aprox 0.5 g on all direction)
					//2438m -> 8,000 ft
					
					buzz(250);
					recoverySelect_t event = DROGUE;
					enable_mosfet(event);
					activate_mosfet(event);
					continuityStatus_t cont = check_continuity(event);
					uint32_t header = (measurement.data[0] << 16) + (measurement.data[1] << 8) + measurement.data[2];
					header |= DROGUE_DETECT;
					measurement.data[0] = (header >> 16) & 0xFF;
					measurement.data[1] = (header >> 8) & 0xFF;
					measurement.data[2] = (header) & 0xFF;
					
					if(cont == OPEN_CIRCUIT)
					{
						configParams->values.state = STATE_IN_FLIGHT_POST_APOGEE;
						configParams->values.flags = configParams->values.flags | 0x08;
						write_config(configParams);
						
						uint32_t header =
							(measurement.data[0] << 16) + (measurement.data[1] << 8) + measurement.data[2];
						header |= DROGUE_DEPLOY;
						measurement.data[0] = (header >> 16) & 0xFF;
						measurement.data[1] = (header >> 8) & 0xFF;
						measurement.data[2] = (header) & 0xFF;
						
					}
					
				}
			}
		}
		
		
		/* Fill Buffer and/or write to flash*********************************************************************************************************/
		is_there_data = isMeasurementEmpty(&measurement);
		
		if(is_there_data && configParams->values.state == STATE_LAUNCHPAD_ARMED &&
		   ((buffer_index_curr + measurement_length + HEADER_SIZE) <= ring_buff_size))
		{
			
			//check if room in launchpad buffer.
			
			
			memcpy(&launchpadBuffer[buffer_index_curr], &(measurement.data), measurement_length + HEADER_SIZE);
			
			buffer_index_curr += (measurement_length + HEADER_SIZE);
			buffer_index_curr = buffer_index_curr % (ring_buff_size);
			
			//Reset the measurement.
			for(i = 0; i < sizeof(Measurement_t); i++)
			{
				
				measurement.data[i] = 0;
			}
		}

//		else if (is_there_data && configParams->values.state == STATE_LAUNCHPAD_ARMED){
//			uint8_t bytesInPrevBuffer = ring_buff_size - buffer_index_curr;
//			uint8_t bytesLeft = (measurement_length+HEADER_SIZE)-bytesInPrevBuffer;
//
//			memcpy(&launchpadBuffer[buffer_index_curr],&(measurement.data),bytesInPrevBuffer);
//			buffer_index_curr = 0;
//			memcpy(&launchpadBuffer[buffer_index_curr],&(measurement.data[bytesInPrevBuffer]),bytesLeft);
//
//			buffer_index_curr = bytesLeft;
//		}
		else if(((buffer_index_curr + measurement_length + HEADER_SIZE) < DATA_BUFFER_SIZE) && (is_there_data))
		{
			
			//There is room in the current buffer for the full measurement.
			
			if(buffer_selection == BUFFER_A)
			{
				
				memcpy(&data_bufferA[buffer_index_curr], &(measurement.data), measurement_length + HEADER_SIZE);
				//transmit_bytes(huart,&data_bufferA[buffer_index_curr],measurement_length+2);
			}else if(buffer_selection == BUFFER_B)
			{
				
				memcpy(&data_bufferB[buffer_index_curr], &(measurement.data), measurement_length + HEADER_SIZE);
				//transmit_bytes(huart,&data_bufferB[buffer_index_curr],measurement_length+2);
			}
			
			buffer_index_curr += (measurement_length + HEADER_SIZE);
			
			//Reset the measurement.
			for(i = 0; i < sizeof(Measurement_t); i++)
			{
				
				measurement.data[i] = 0;
			}
			
			
		}else if(is_there_data)
		{
			
			//Split measurement across the buffers, and write to flash.
			uint8_t bytesInPrevBuffer = DATA_BUFFER_SIZE - buffer_index_curr;
			uint8_t bytesLeft = (measurement_length + HEADER_SIZE) - bytesInPrevBuffer;

//			if((((measurement.data[1]<<8)+measurement.data[0])&0x0FFF)>300){
//
//				while(1);
//			}

//			if((bytesLeft+bytesInPrevBuffer)!=20 && ((bytesLeft+bytesInPrevBuffer)!=14)){
//
//				while(1){}
//			}
			
			//Put as much data as will fit into the almost full buffer.
			if(buffer_selection == BUFFER_A)
			{
				
				memcpy(&data_bufferA[buffer_index_curr], &(measurement.data), bytesInPrevBuffer);
				buffer_selection = BUFFER_B;
				buffer_index_curr = 0;
				
			}else if(buffer_selection == BUFFER_B)
			{
				
				memcpy(&data_bufferB[buffer_index_curr], &(measurement.data), bytesInPrevBuffer);
				buffer_index_curr = BUFFER_A;
				buffer_selection = 0;
				
			}
			
			//Put the rest of the measurement in the next buffer.
			if(buffer_selection == BUFFER_A)
			{
				
				memcpy(&data_bufferA[buffer_index_curr], &(measurement.data[bytesInPrevBuffer]), bytesLeft);
				buffer_index_curr = bytesLeft;
				
			}else if(buffer_selection == BUFFER_B)
			{
				
				memcpy(&data_bufferB[buffer_index_curr], &(measurement.data[bytesInPrevBuffer]), bytesLeft);
				buffer_index_curr = bytesLeft;
				
			}
			
			//reset the measurement.
			for(i = 0; i < sizeof(Measurement_t); i++)
			{
				
				measurement.data[i] = 0;
			}
			
			measurement_length = 0;
			
			//Flash write buffer not in use! then clear old buffer?
			
			if(buffer_selection == 0)
			{
				//We just switched to A so transmit B.
				
				if(IS_RECORDING(configParams->values.flags))
				{
					
					FlashStatus stat_f = flash_program_page(flash_ptr, flash_address, data_bufferB, DATA_BUFFER_SIZE);
					while(FLASH_IS_DEVICE_BUSY(stat_f))
					{
						stat_f = flash_get_status_register(flash_ptr);
						vTaskDelay(1);
					}
					
					flash_address += DATA_BUFFER_SIZE;
					if(flash_address >= FLASH_SIZE_BYTES)
					{
						while(1);
					}
					
				}else
				{
					transmit_bytes(huart, data_bufferB, 256);
				}
			}else if(buffer_selection == 1)
			{
				//We just switched to B so transmit A
				
				if(IS_RECORDING(configParams->values.flags))
				{
					FlashStatus stat_f2 = flash_program_page(flash_ptr, flash_address, data_bufferA, DATA_BUFFER_SIZE);
					while(FLASH_IS_DEVICE_BUSY(stat_f2))
					{
						stat_f2 = flash_get_status_register(flash_ptr);
						vTaskDelay(1);
					}
					
					flash_address += DATA_BUFFER_SIZE;
					
					if(flash_address >= FLASH_SIZE_BYTES)
					{
						while(1);
					}
				}else
				{
					transmit_bytes(huart, data_bufferA, 256);
				}
			}
			
			
		}
		
		for(i = 0; i < sizeof(Measurement_t); i++)
		{
			
			measurement.data[i] = 0;
		}
		
		measurement_length = 0;
		
		if(!running)
		{
			vTaskSuspend(NULL);
		}
	};
	
}
