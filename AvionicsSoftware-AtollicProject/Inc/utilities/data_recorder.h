#ifndef AVIONICS_DATA_RECORDER_H
#define AVIONICS_DATA_RECORDER_H

#include "utilities/common.h"
#include "tasks/sensors/imu_sensor.h"
#include "tasks/sensors/pressure_sensor.h"

typedef struct{
	uint8_t data[HEADER_SIZE+ACC_LENGTH+GYRO_LENGTH+PRES_LENGTH+TEMP_LENGTH+ALT_LENGTH];
}data_measurement;

#define ACC_TYPE 			0x800000
#define GYRO_TYPE			0x400000
#define PRES_TYPE			0x200000
#define	TEMP_TYPE			0x100000

static inline void process_imu_data(imu_sensor_data imu_reading, uint8_t* dest, uint32_t timestamp)
{
	// Make sure time doesn't overwrite type and event bits.
	uint32_t header = (ACC_TYPE | GYRO_TYPE) + (timestamp & 0x0FFF);
	write_24(header,                  &dest[0]);
	write_16(imu_reading.accel.x, &dest[3]);
	write_16(imu_reading.accel.y, &dest[5]);
	write_16(imu_reading.accel.z, &dest[7]);
	write_16(imu_reading.gyro.x, &dest[9]);
	write_16(imu_reading.gyro.y, &dest[11]);
	write_16(imu_reading.gyro.z, &dest[13]);
}

static inline float process_bmp_data_and_return_altitude(pressure_sensor_data bmp_reading, uint8_t * dest, float ref_pres, float ref_alt)
{
	//Update the header bytes.
	uint32_t header = (dest[0] << 16) + (dest[1] << 8) + dest[2];
	header |= PRES_TYPE | TEMP_TYPE;
	
	altitude_data altitude = calculate_altitude((float) bmp_reading.data.pressure, (float) bmp_reading.data.temperature,
											ref_pres, ref_alt);
	
	write_24(header, &dest[0]);
	write_24(bmp_reading.data.pressure,    &dest[15]);
	write_24(bmp_reading.data.temperature, &dest[18]);
	write_32(altitude.byte_val,            &dest[21]);
	
	return altitude.float_val;
}

#endif //AVIONICS_DATA_RECORDER_H
