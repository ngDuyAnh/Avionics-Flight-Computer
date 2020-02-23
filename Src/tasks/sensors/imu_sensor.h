#ifndef SENSOR_AG_H
#define SENSOR_AG_H
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// UMSATS 2018-2020
//
// Repository:
//  ?Not this:UMSATS Google Drive: UMSATS/Guides and HowTos.Command and Data Handling (CDH)/Coding Standards
//
// File Description:
//  Reads sensor data for accelerometer and gyroscope from the BMI088
//
// History
// 2019-03-29 by Benjamin Zacharias
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#include <inttypes.h>
#include <stdbool.h>
#include "configuration.h"
#include "protocols/UART.h"

#define ACC_LENGTH  6 // Length of a accelerometer measurement in bytes.
#define GYRO_LENGTH 6 // Length of a gyroscope measurement in bytes.


//Groups both sensor readings and a time stamp.
typedef struct imu_sensor_data
{
    int16_t    acc_x;
    int16_t    acc_y;
    int16_t    acc_z;
    
    int16_t    gyro_x;
    int16_t    gyro_y;
    int16_t    gyro_z;
    
    uint32_t   time_ticks; // time of sensor reading in ticks.
}imu_sensor_data;

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// Description:
//  Enter description for public function here.
//
// Returns:
//  Enter description of return values (if any).
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

int imu_sensor_test            ();
int imu_sensor_init            (configuration_data_t * parameters);
void imu_thread_start           (void const *param);
bool imu_read                   (imu_sensor_data * buffer, uint8_t data_rate);
void imu_sensor_data_to_bytes   (imu_sensor_data reading, uint8_t* bytes, uint32_t timestamp);

bool imu_add_measurement        (imu_sensor_data *_data);


#endif // SENSOR_AG_H
