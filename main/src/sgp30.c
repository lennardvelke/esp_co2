#include "sgp30.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_wrapper.h"
#include <stdio.h>

esp_err_t sgp30_execute_command(sgp30_sensor *sensor, uint8_t command[], uint8_t command_length, uint16_t delay, uint8_t *read_data, uint8_t read_data_length)
{
    esp_err_t error = ESP_OK;

    error = i2c_master_transmit(*sensor->dev_handle, command, command_length, -1);

    vTaskDelay(delay);

    if (read_data_length > 0)
    {
        error = i2c_master_receive(*sensor->dev_handle, read_data, read_data_length, -1);
        // if (read_data[read_data_length - 1] != sgp30_calculate_CRC(read_data, read_data_length - 1))
        // {
        //     printf("The checksum doesn't match with the recieved one: \n    Expected Checksum: %d \n    Recieved Checksum: %d\n", sgp30_calculate_CRC(read_data, read_data_length - 1), read_data[read_data_length - 1]);
        // }
    }

    return error;
}

void sgp30_init(sgp30_sensor *sensor)
{
    uint16_t command = SGP30_GET_FEATURE_SET;
    uint8_t x = (command >> (8 * 0)) & 0xff;
    uint8_t y = (command >> (8 * 1)) & 0xff;

    printf("%d, %d \n", x, y);
    uint8_t write_buffer[2] = {x, y};
    uint8_t read_buffer[3] = {0};

    sgp30_execute_command(sensor, write_buffer, 2, 10, read_buffer, 3);

    printf("The Feature set ID is %d", read_buffer[1]);
    return;
}

void sgp30_iaq_init(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(SGP30_IAQ_INIT >> 8) & 0xff, (uint8_t)(SGP30_IAQ_INIT >> 0) & 0xff};

    sgp30_execute_command(sensor, writebuffer, 2, 10, NULL, 0);
}

sgp30_return_values_measure_iaq sgp30_measure_iaq(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(uint8_t)(SGP30_MEASURE_IAQ >> 8) & 0xff, (uint8_t)(SGP30_MEASURE_IAQ >> 0) & 0xff};
    uint8_t readbuffer[6] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 12, readbuffer, 6);

    uint16_t co2_result = (readbuffer[0] << 8) + readbuffer[1];
    uint16_t tvoc_result = (readbuffer[4] << 8) + readbuffer[3];

    return (sgp30_return_values_measure_iaq){
        co2_result,
        readbuffer[2],
        tvoc_result,
        readbuffer[5],
    };
}

void sgp30_get_iaq_baseline(sgp30_sensor *sensor)
{
}

void sgp30_set_absolute_humidity(sgp30_sensor *sensor)
{
}

void sgp30_set_iaq_baseline(sgp30_sensor *sensor)
{
}

sgp30_return_values_measure_test sgp30_measure_test(sgp30_sensor *sensor)
{

    uint8_t writebuffer[2] = {(SGP30_MEASURE_TEST >> 8) & 0xff, (uint8_t)(SGP30_MEASURE_TEST >> 0) & 0xff};
    uint8_t readbuffer[3] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 220, readbuffer, 3);

    // printf("Results for measure_test: %d, %d, %d\n", readbuffer[0], readbuffer[1], readbuffer[2]);
    // printf("CRC of measure_test: %d \n", sgp30_calculate_CRC(readbuffer, 2));
    uint16_t fixed_data_pattern = (readbuffer[0] << 8) + readbuffer[1];

    return (sgp30_return_values_measure_test){
        fixed_data_pattern,
        readbuffer[2],
    };
}

void sgp30_get_feature_set(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(SGP30_GET_FEATURE_SET >> 8) & 0xff, (uint8_t)(SGP30_GET_FEATURE_SET >> 0) & 0xff};
    uint8_t readbuffer[3] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 220, readbuffer, 3);

    // printf("Results for get_feature_set: %d, %d, %d\n", readbuffer[0], readbuffer[1], readbuffer[2]);
    // printf("CRC of get_feature_set: %d \n", sgp30_calculate_CRC(readbuffer, 2));
}

void sgp30_measure_raw(sgp30_sensor *sensor)
{
}

void sgp30_get_tvoc_inceptive_baseline(sgp30_sensor *sensor)
{
}

void sgp30_set_tvoc_baseline(sgp30_sensor *sensor)
{
}

uint8_t sgp30_calculate_CRC(uint8_t *data, uint8_t len)
{
    /**
     ** Data and commands are protected with a CRC checksum to increase the communication reliability.
     ** The 16-bit commands that are sent to the sensor already include a 3-bit CRC checksum.
     ** Data sent from and received by the sensor is always succeeded by an 8-bit CRC.
     *! In write direction it is mandatory to transmit the checksum, since the SGP30 only accepts data if
     *! it is followed by the correct checksum.
     *
     ** In read direction it is up to the master to decide if it wants to read and process the checksum
     */
    uint8_t crc = SGP30_CRC8_INIT;

    for (uint8_t i = 0; i < len; i++)
    {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++)
        {
            if (crc & 0x80)
                crc = (crc << 1) ^ SGP30_CRC8_POLYNOMIAL;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void sgp30_print(void *result_pointer, const uint16_t type)
{

    switch (type)
    {
    case SGP30_MEASURE_IAQ:
        sgp30_return_values_measure_iaq result = *(sgp30_return_values_measure_iaq *)(result_pointer);
        printf("Results for measure_iaq:\n     The CO2 Data is: %d;\n     The CRC of CO2 is: %d;\n     The Tvoc Data is: %d;\n     The CRC of Tvoc is: %d;\n", result.co2_result, result.crc_co2, result.tvoc_result, result.crc_tvoc);
        break;
    case SGP30_MEASURE_TEST:
        sgp30_return_values_measure_test result2 = *(sgp30_return_values_measure_test *)(result_pointer);
        printf("Results for measure_test:\n     The fixed Data Pattern is: %d;\n     The CRC is: %d;\n", result2.fixed_data_pattern, result2.crc);
        break;

    default:
        break;
    }
}
