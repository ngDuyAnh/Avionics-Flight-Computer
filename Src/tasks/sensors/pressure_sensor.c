//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// pressure_sensor_bmp3.c
// UMSATS 2018-2020
//
// Repository:
//  UMSATS > Avionics 2019
//
// File Description:
//  Control and usage of BMP3 sensor inside of RTOS task.
//
// History
// 2019-04-06 Eric Kapilik
// - Created.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
// INCLUDES
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
#include "pressure_sensor.h"
#include <math.h>
#include <stdbool.h>

#include "../../external/sensors/bmp3.h"
#include "cmsis_os.h"
#include "../../protocols/SPI.h"
#include "../../configuration.h"
#include "../../protocols/UART.h"
#include "utilities/common.h"

#define INTERNAL_ERROR      -127
#define PRES_TYPE           0x200000
#define TEMP_TYPE           0x100000

#define GND_ALT             0
#define GND_PRES            101325

static float s_reference_pressure = 0.0f;
static float s_reference_altitude = 0.0f;

// Keep SPI connection and BMP sensor struct together
typedef struct _bmp3_sensor_struct
{
    struct bmp3_dev *bmp_ptr;
    SPI hspi_ptr;
} _bmp3_sensor;
static UART uart;
static char buf[128];
static _bmp3_sensor *s_bmp3_sensor;
static QueueHandle_t bmp388_queue;
static struct bmp3_data sensor_data;
static struct bmp3_dev bmp3;
static _bmp3_sensor bmp3_sensor;

static void     delay_ms                    (uint32_t period_ms);
static int8_t   spi_reg_write               (uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
static int8_t   spi_reg_read                (uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length);
int8_t          __pressure_sensor_init      (_bmp3_sensor *bmp3_sensor_ptr);
static bool     __pressure_sensor_config    (uint8_t iir, uint8_t os_pres, uint8_t os_temp, uint8_t odr);
static int8_t   get_sensor_data             (struct bmp3_dev *dev, struct bmp3_data *data);
static void     bmp3_print_result           (const char *api_name, int8_t rslt);



int8_t __pressure_sensor_init(_bmp3_sensor *bmp3_sensor_ptr)
{
    struct bmp3_dev *bmp3_ptr;
    SPI hspi_ptr = spi2_init();
    if(hspi_ptr == NULL)
    {
        return INTERNAL_ERROR;
    }
    
    //Initialize BMP3 Handler
    bmp3_ptr = &bmp3;
    if(bmp3_ptr == NULL)
    {
        return INTERNAL_ERROR;
    }
    
    /* Set bmp3_sensor_ptr members to newly initialized handlers */
    bmp3_sensor_ptr->bmp_ptr = bmp3_ptr;
    bmp3_sensor_ptr->hspi_ptr = hspi_ptr;
    
    // Save static reference to bmp3_sensor_ptr for use in spi_reg_read/write wrapper functions
    // The spi_reg_read/write functions have function signatures defined by the BOSCH API which they conform to.
    // Unfortunately, a reference to the SPI connection is not in that signature, so I keep a static reference to it for use in said wrapper functions.
    s_bmp3_sensor = bmp3_sensor_ptr;
    
    /* Map the delay function pointer with the function responsible for implementing the delay_ms */
    bmp3_ptr->delay_ms = delay_ms;
    
    /* Select the interface mode as SPI */
    bmp3_ptr->intf = BMP3_SPI_INTF;
    bmp3_ptr->read = spi_reg_read;
    bmp3_ptr->write = spi_reg_write;
    bmp3_ptr->dev_id = 0;
    
    int8_t result = bmp3_init(bmp3_ptr); // bosch API initialization method
    
    if(result == BMP3_OK)
    {
        bmp388_queue = xQueueCreate(10, sizeof(pressure_sensor_data));
        if(bmp388_queue == NULL)
        {
            return INTERNAL_ERROR;
        }
        
        vQueueAddToRegistry(bmp388_queue,"bmp388_queue");
    }else
    {
        return result;
    }
    
    return INTERNAL_ERROR;
}
static bool __pressure_sensor_config(uint8_t iir, uint8_t os_pres, uint8_t os_temp, uint8_t odr)
{
    struct bmp3_dev *dev = s_bmp3_sensor->bmp_ptr;
    
    /* Used to select the settings user needs to change */
    uint16_t settings_sel;
    
    /* Select the pressure and temperature sensor to be enabled */
    dev->settings.press_en = BMP3_ENABLE;
    dev->settings.temp_en = BMP3_ENABLE;
    /* Select the output data rate and oversampling settings for pressure and temperature */
    dev->settings.odr_filter.press_os = os_pres;
    dev->settings.odr_filter.temp_os = os_temp;
    dev->settings.odr_filter.odr = odr;
    dev->settings.odr_filter.iir_filter = iir;
    /* Assign the settings which needs to be set in the sensor */
    settings_sel = BMP3_PRESS_EN_SEL | BMP3_TEMP_EN_SEL | BMP3_PRESS_OS_SEL | BMP3_TEMP_OS_SEL | BMP3_ODR_SEL |
                   BMP3_IIR_FILTER_SEL;
    if(0 == bmp3_set_sensor_settings(settings_sel, dev))
    {
        return false;
    }
    
    /* Set the power mode to normal mode */
    dev->settings.op_mode = BMP3_NORMAL_MODE;
    
    return (bmp3_set_op_mode(dev) == 0) ? true : false;
}

float pressure_sensor_calculate_altitude(pressure_sensor_data * reading)
{
    if(reading == NULL)
    {
        return 0.0;
    }
    
    float p_term = powf((s_reference_pressure / (reading->pressure / 100)), (1 / 5.257F)) - 1;
    float t_term = (reading->temperature / 100) + 273.15F;
    return (uint32_t) (p_term * t_term) / 0.0065F + s_reference_altitude;
}

int8_t get_sensor_data(struct bmp3_dev *dev, struct bmp3_data *data)
{
    return bmp3_get_sensor_data(BMP3_PRESS | BMP3_TEMP, data, dev);
}

bool pressure_sensor_init(configuration_data_t *parameters)
{
    _bmp3_sensor *bmp3_sensor_ptr = &bmp3_sensor;
    if(bmp3_sensor_ptr == NULL)
    {
        return false;
    }
    
    if(false == __pressure_sensor_init(bmp3_sensor_ptr))
    {
        return false;
    }
    
    
    
    return __pressure_sensor_config(parameters->values.iir_coef,
                                    parameters->values.pres_os,
                                    parameters->values.temp_os,
                                    parameters->values.bmp_odr);
    
}
void pressure_sensor_calibrate(configuration_data_t *configParams)
{
    pressure_sensor_data dataStruct;
    get_sensor_data(s_bmp3_sensor->bmp_ptr, &sensor_data);
    
    dataStruct.pressure = sensor_data.pressure;
    dataStruct.temperature = sensor_data.temperature;
    
    configParams->values.ref_alt = 0;
    configParams->values.ref_pres = (uint32_t) dataStruct.pressure / 100;
    s_reference_pressure = configParams->values.ref_pres;
    s_reference_altitude = configParams->values.ref_alt;
}
void thread_pressure_sensor_start(void const*pvParameters)
{
    pressure_sensor_thread_parameters *params = (pressure_sensor_thread_parameters *) pvParameters;
    uart = params->huart;    //Get uart for printing to console
    configuration_data_t *configParams = params->flightCompConfig;
    /* Variable used to store the compensated data */
    pressure_sensor_data dataStruct;
    TickType_t prevTime;
    prevTime = xTaskGetTickCount();
    
    for(size_t i = 0; i < 3; i++)
    {
        get_sensor_data(s_bmp3_sensor->bmp_ptr, &sensor_data);
        dataStruct.pressure = sensor_data.pressure;
        dataStruct.temperature = sensor_data.temperature;
        
        vTaskDelayUntil(&prevTime, configParams->values.data_rate);
    }
    
    if(!CONFIGURATION_IS_IN_FLIGHT(configParams->values.flags))
    {
        get_sensor_data(s_bmp3_sensor->bmp_ptr, &sensor_data);
        dataStruct.pressure = sensor_data.pressure;
        dataStruct.temperature = sensor_data.temperature;
        
        configParams->values.ref_pres = dataStruct.pressure / 100;
        s_reference_pressure = dataStruct.pressure / 100;
    }
    
    int8_t result_flag;
    while(1)
    {
        
        result_flag = get_sensor_data(s_bmp3_sensor->bmp_ptr, &sensor_data);
        if(BMP3_E_NULL_PTR == result_flag)
        {
            continue;
        }

        dataStruct.pressure = sensor_data.pressure;
        dataStruct.temperature = sensor_data.temperature;
        
        dataStruct.time_ticks = xTaskGetTickCount();

        pressure_sensor_add_measurement(&dataStruct);
        vTaskDelayUntil(&prevTime, configParams->values.data_rate);
    }
}

static void delay_ms(uint32_t period_ms)
{
    vTaskDelay((TickType_t) period_ms);
}

/*!
 *  @brief Function for writing the sensor's registers through SPI bus.
 *
 *  @param[in] cs           : Chip select to enable the sensor.
 *  @param[in] reg_addr     : Register address.
 *  @param[in] reg_data : Pointer to the data buffer whose data has to be written.
 *  @param[in] length       : No of bytes to write.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
static int8_t spi_reg_write(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{
    int8_t rslt = 0; //assume success
    spi_send(s_bmp3_sensor->hspi_ptr, &reg_addr, 1, reg_data, length, TIMEOUT);
    return rslt;
}
/*!
 *  @brief Function for reading the sensor's registers through SPI bus.
 *
 *  @param[in] cs       : Chip select to enable the sensor.
 *  @param[in] reg_addr : Register address.
 *  @param[out] reg_data    : Pointer to the data buffer to store the read data.
 *  @param[in] length   : No of bytes to read.
 *
 *  @return Status of execution
 *  @retval 0 -> Success
 *  @retval >0 -> Failure Info
 *
 */
static int8_t spi_reg_read(uint8_t cs, uint8_t reg_addr, uint8_t *reg_data, uint16_t length)
{
    int8_t rslt = 0; // assume success
    spi_receive(s_bmp3_sensor->hspi_ptr, &reg_addr, 1, reg_data, length, TIMEOUT);
    return rslt;
}
/*!
 *  @brief Prints the execution status of the APIs.
 *
 *  @param[in] api_name : name of the API whose execution status has to be printed.
 *  @param[in] rslt     : error code returned by the API whose execution status has to be printed.
 *
 *  @return void.
 */
void bmp3_print_result(const char *api_name, int8_t rslt)
{
    if(rslt != BMP3_OK)
    {
        char error_msg[64];
        if(rslt == BMP3_E_NULL_PTR)
        {
            sprintf(error_msg, "Null pointer error");
        }else if(rslt == BMP3_E_DEV_NOT_FOUND)
        {
            sprintf(error_msg, "Device not found");
        }else if(rslt == BMP3_E_INVALID_ODR_OSR_SETTINGS)
        {
            sprintf(error_msg, "Invalid ODR OSR settings");
        }else if(rslt == BMP3_E_CMD_EXEC_FAILED)
        {
            sprintf(error_msg, "Command execution failed");
        }else if(rslt == BMP3_E_CONFIGURATION_ERR)
        {
            sprintf(error_msg, "Configuration error");
        }else if(rslt == BMP3_E_INVALID_LEN)
        {
            sprintf(error_msg, "Invalid length");
        }else if(rslt == BMP3_E_COMM_FAIL)
        {
            sprintf(error_msg, "Communication failure");
        }else if(rslt == BMP3_E_FIFO_WATERMARK_NOT_REACHED)
        {
            sprintf(error_msg, "FIFO Watermark not reached");
        }else
        {
            //For more error codes refer "bmp3_defs.h"
            sprintf(error_msg, "Unknown error code");
        }
        sprintf(buf, "\r\nERROR [%d] %s : %s\r\n", rslt, api_name, error_msg);
    }
    
    uart_transmit(uart, buf);
}


bool pressure_sensor_test(void)
{
    char result = 0;
    uint8_t id= 0x50;
    
    uint8_t command[] = {0x80};
    uint8_t id_read[] = {0x00,0x00};
    
    spi_receive(s_bmp3_sensor->hspi_ptr,command,1,id_read,2,10);
    
    if(id_read[1] == id)
    {
        result = 1;
    }
    
    return result == 1;
}

bool pressure_sensor_read(pressure_sensor_data * buffer, uint8_t data_rate)
{
    return pdPASS == xQueueReceive(bmp388_queue, buffer, data_rate);
}


void pressure_sensor_data_to_bytes(pressure_sensor_data bmp_reading, uint8_t * bytes)
{
    //Update the header bytes.
    uint32_t header = (bytes[0] << 16) + (bytes[1] << 8) + bytes[2];
    header |= PRES_TYPE | TEMP_TYPE;
    
    write_24(header, &bytes[0]);
    write_24(bmp_reading.pressure,    &bytes[15]);
    write_24(bmp_reading.temperature, &bytes[18]);
    float altitude = pressure_sensor_calculate_altitude(&bmp_reading);
    float2bytes(altitude, &bytes[21]);
}

bool pressure_sensor_add_measurement (pressure_sensor_data * _data)
{
    return pdTRUE == xQueueSend(bmp388_queue,_data,1);
}
