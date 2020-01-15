/*    Avionics Software
 *
 *    File Description:
 *        Main file for the avionics software. The tasks are initialized here and the scheduler is started.
 *        Right now the default task is setup to blink the on-board LED.
 *
 *
 *    History:
 *    - 2019-01-22
 *        Created by Joseph Howarth
 *
 *
 */

#include <stddef.h>
#include <stdio.h>
#include "tasks/app_threads_controller.h"
#include <stm32f401xe.h>

#include "stm32/STM32.h"
#include "protocols/UART.h"
#include "buzzer.h"
#include "flash.h"

#include "configuration.h"

#include "recovery.h"

#include "tasks/sensors/imu_sensor.h"
#include "tasks/sensors/pressure_sensor.h"
#include "tasks/command_line_interface/controller.h"
#include "tasks/flight_state_controller.h"
#include "tasks/timer.h"
#include "cmsis_os.h"


int main(void)
{
    configuration_data_t app_configuration_data;
    STM32Status board_status = stm32_init();
    if(board_status != STM32_OK)
    {
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    UART huart6 = UART_Port6_Init();
    if(huart6 == NULL)
    {
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    uart_transmit_line(huart6, "UMSATS ROCKETRY FLIGHT COMPUTER");
    
    Flash flash = flash_initialize();
    if(flash == NULL)
    {
        stm32_error_handler(__FILE__, __LINE__);
    }
    app_configuration_data.values.flash = flash;
    
    uart_transmit_line(huart6, "Flash ID read successful");
    
    read_config(&app_configuration_data);
    
    char lines[50];
    sprintf(lines, "ID :%d \n", app_configuration_data.values.id);
    uart_transmit_line(huart6, lines);
    
    if(app_configuration_data.values.id != ID)
    {
        uart_transmit_line(huart6, "No config found in flash, resetting to default.");
        init_config(&app_configuration_data);
        write_config(&app_configuration_data);
    }
    
    read_config(&app_configuration_data);
    
    if(CONFIGURATION_IS_POST_MAIN(app_configuration_data.values.flags))
    {
        app_configuration_data.values.state = STATE_IN_FLIGHT_POST_MAIN;
    }else if(CONFIGURATION_IS_POST_DROGUE(app_configuration_data.values.flags))
    {
        app_configuration_data.values.state = STATE_IN_FLIGHT_POST_APOGEE;
    }else if(CONFIGURATION_IS_PRE_DROGUE(app_configuration_data.values.flags))
    {
        app_configuration_data.values.state = STATE_IN_FLIGHT_PRE_APOGEE;
    }
    
    size_t end_Address = flash_scan(flash);
    sprintf(lines, "end address :%zu", end_Address);
    uart_transmit_line(huart6, lines);
    app_configuration_data.values.end_data_address = end_Address;
    
    buzzer_init();
    uart_transmit_line(huart6, "Buzzer has been set up.");
    
    recovery_init();
    uart_transmit_line(huart6, "Recovery GPIO pins have been set up.");
    
    //Initialize and get the flight computer parameters.
    imu_sensor_thread_parameters thread_imu_params;
    pressure_sensor_thread_parameters thread_pressure_sensor_params;
    flight_state_controller_thread_parameters thread_flight_state_controller_params;
    cli_thread_parameters thread_cli_params;
    startup_thread_parameters thread_startup_parameters;
    
    thread_flight_state_controller_params.flash_ptr = flash;
    thread_flight_state_controller_params.uart = huart6;
    thread_flight_state_controller_params.configuration_data = &app_configuration_data;
    
    
    thread_pressure_sensor_params.huart = huart6;
    thread_pressure_sensor_params.flightCompConfig = &app_configuration_data;
    pressure_sensor_init(&app_configuration_data);
    
    
    thread_imu_params.huart = huart6;
    thread_imu_params.configuration_data = &app_configuration_data;
    imu_sensor_init(&app_configuration_data);
    
    
    thread_cli_params.flash = flash;
    thread_cli_params.huart = huart6;
    thread_cli_params.application_configurations = &app_configuration_data;
    task_command_line_controller_init(&thread_cli_params);
    
    thread_startup_parameters.flight_state_controller_thread_handle = NULL;
    thread_startup_parameters.pressure_sensor_thread_handle         = NULL;
    thread_startup_parameters.imu_thread_handle                     = NULL;
    thread_startup_parameters.cli_thread_params                     = NULL;
    thread_startup_parameters.timer_thread_handle                   = NULL;
    thread_cli_params.startupTaskHandle                             = NULL;
    
    if(!CONFIGURATION_IS_IN_FLIGHT(app_configuration_data.values.flags))
    {
        app_configuration_data.values.state = STATE_LAUNCHPAD;
        if(imu_sensor_test() && pressure_sensor_test())
        {
            stm32_delay(1000);
            buzz(500); //CHANGE TO 2 SECONDS !!!!!
        }
        else
        {
            for(uint8_t i = 0; i < 20; i++)
            {
                buzz(500);
                stm32_delay(500);
                app_configuration_data.values.state = STATE_CLI;
            }
        }
    }
    
    osThreadDef(timer, thread_timer_start, osPriorityAboveNormal, 1, 1000);
    if(NULL == (thread_startup_parameters.timer_thread_handle = osThreadCreate(osThread(timer), &app_configuration_data))){
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    osThreadDef(imu, imu_thread_start, osPriorityHigh, 1, 1000);
    if(NULL == (thread_startup_parameters.imu_thread_handle = osThreadCreate(osThread(imu), &app_configuration_data))){
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    osThreadDef(flight_state_controller, thread_flight_state_controller_start, osPriorityHigh, 1, 1000);
    if(NULL == (thread_startup_parameters.flight_state_controller_thread_handle = osThreadCreate(osThread(flight_state_controller), &thread_flight_state_controller_params))){
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    osThreadDef(cli, thread_command_line_controller_start, osPriorityAboveNormal, 1, 1000);
    if(NULL == (thread_startup_parameters.cli_thread_params = osThreadCreate(osThread(cli), NULL))){
        stm32_error_handler(__FILE__, __LINE__);
    }

    osThreadDef(pressure_sensor, thread_pressure_sensor_start, osPriorityAboveNormal, 1, 1000);
    if(NULL == (thread_startup_parameters.pressure_sensor_thread_handle = osThreadCreate(osThread(pressure_sensor), &app_configuration_data))){
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    osThreadDef(startup, app_threads_controller_start, osPriorityAboveNormal, 1, 1000);
    if(NULL == (thread_cli_params.startupTaskHandle = osThreadCreate(osThread(startup), &app_configuration_data))){
        stm32_error_handler(__FILE__, __LINE__);
    }
    
    //Start with all tasks suspended except starter task.
    osThreadSuspend(thread_startup_parameters.cli_thread_params);
    osThreadSuspend(thread_startup_parameters.imu_thread_handle);
    osThreadSuspend(thread_startup_parameters.pressure_sensor_thread_handle);
    osThreadSuspend(thread_startup_parameters.flight_state_controller_thread_handle);
    osThreadSuspend(thread_startup_parameters.timer_thread_handle);
    
    
    /* Start scheduler -- comment to not use FreeRTOS */
    osKernelStart();
    
    /* We should never get here as control is now taken by the scheduler */
    stm32_error_handler(__FILE__, __LINE__);
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
//void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
//{
//  /* USER CODE BEGIN Callback 0 */
//
//  /* USER CODE END Callback 0 */
//  if (htim->Instance == TIM1) {
//    HAL_IncTick();
//  }
//
//}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{

  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
