//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS 2018-2020
//
// Repository:
//  UMSATS/Avionics-2019
//
// File Description:
//  Source file for the start up task.
//
// History
// 2019-04-19 by Joseph Howarth
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "tasks/startup.h"

#include "cmsis_os.h"
#include "configuration.h"
#include "flash.h"
#include "hardware_definitions.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  This task will check if the memory is empty and erase it if it is not.
//
//	Right now the function assumes that if the first page of memory is empty,
//	then the whole memory is empty.
//
// Returns:
//

static void eraseFlash(startup_thread_parameters * params){

	  HAL_GPIO_WritePin(USR_LED_PORT,USR_LED_PIN,GPIO_PIN_RESET);

	  FlashStatus stat;

	  Flash flash = params->flash_ptr;
	  UART  huart = params->huart_ptr;
	  configuration_data_t * config = params->configuration_data;

	  uint8_t dataRX[256];
	  uart_transmit_line(huart,"Checking flash memory...");
	 // Read the first page of memory. If its empty, assume the whole memory is empty.
	  stat = flash_read_page(flash,config->values.start_data_address,dataRX,256);

	  uint16_t good= 0xFFFF;

	  int i;
	  for(i=0;i<256;i++){

		  if(dataRX[i] != 0xFF){
			  good --;
		  }
	  }

	  if(good == 0xFFFF){
		  		  uart_transmit_line(huart,"flash empty.");
		  		  HAL_GPIO_WritePin(USR_LED_PORT,USR_LED_PIN,GPIO_PIN_RESET);

	  }else{


		  uart_transmit_line(huart,"flash not empty.");
		  //Erase the whole flash. This could take up to 2 minutes.
	  	  //stat = erase_device(flash);
		  uint32_t address = FLASH_START_ADDRESS;

		  while(address < config->values.end_data_address){

			  if(address>FLASH_PARAM_END_ADDRESS){
			  stat = flash_erase_sector(flash,address);
			  address += FLASH_SECTOR_SIZE;
			  }
			  else{
				  stat = flash_erase_param_sector(flash,address);
				  address += FLASH_PARAM_SECTOR_SIZE;
			  }
			  //Wait for erase to finish
			  while(FLASH_IS_DEVICE_BUSY(stat)){

				  stat = flash_get_status_register(flash);

				  vTaskDelay(pdMS_TO_TICKS(1));
			  }


		  }

		  flash_read_page(flash,FLASH_START_ADDRESS,dataRX,256);
		  uint16_t empty = 0xFFFF;

		  for(i=0;i<256;i++){

			  if(dataRX[i] != 0xFF){
				 empty --;
			  }
		  }

		  if(empty == 0xFFFF){

			  uart_transmit_line(huart,"Flash Erased Success!");
			  HAL_GPIO_WritePin(USR_LED_PORT,USR_LED_PIN,GPIO_PIN_SET);
			  HAL_Delay(1000);
			  HAL_GPIO_WritePin(USR_LED_PORT,USR_LED_PIN,GPIO_PIN_RESET);
		  }

	  }
}


void thread_startup_start(void const* pvParams){

	startup_thread_parameters * sp = (startup_thread_parameters *) pvParams;

	osThreadId dataLoggingTask_h = sp->flight_state_controller_thread_handle;
	osThreadId bmpTask_h = sp->pressure_sensor_thread_handle;
	osThreadId imuTask_h = sp->imu_thread_handle;
	osThreadId cliTask_h = sp->cli_thread_params;
	Flash flash = sp->flash_ptr;
	UART huart = sp->huart_ptr;
	configuration_data_t * config = sp->configuration_data;

	osThreadSuspend(cliTask_h);
	osThreadSuspend(imuTask_h);
	osThreadSuspend(bmpTask_h);
	osThreadSuspend(dataLoggingTask_h);

	  while(1){

		  if((!HAL_GPIO_ReadPin(USR_PB_PORT,USR_PB_PIN))||(config->values.state == STATE_CLI)){

			  HAL_GPIO_WritePin(USR_LED_PORT,USR_LED_PIN,GPIO_PIN_SET);
			  config->values.state = STATE_CLI;
			  osThreadResume(cliTask_h);
			  osThreadSuspend(NULL);

		  }else{

			  if(!IS_IN_FLIGHT(config->values.flags)){

				  uint32_t count = 0;
				  uint32_t delay_ms = 2000;
				  uint8_t state = 0;

				  uint32_t time = config->values.initial_time_to_wait;

				  while(count<time){

					  vTaskDelay(pdMS_TO_TICKS(delay_ms));
					  HAL_GPIO_TogglePin(USR_LED_PORT,USR_LED_PIN);
					  count += delay_ms;

					  if(count > (time/2) && state ==0){

						  delay_ms = 1000;
						  state = 1;
					  }
					  else if( count > (3*(time/4)) && state == 1){

						  delay_ms = 500;
						  state = 2;
					  }

				  }

			  	  eraseFlash(sp);
			  }

			  osThreadResume(dataLoggingTask_h);
			  osThreadResume(imuTask_h);
			  osThreadResume(bmpTask_h);
			  osThreadSuspend(NULL);
		  }
	  }

}
