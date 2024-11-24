#include "sgp30.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "i2c_wrapper.h"
#include <stdio.h>
#include <string.h>

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
    sgp30_result_get_feature_set future_set = sgp30_get_feature_set(sensor);
    sgp30_result_get_serial_id serial_id = sgp30_get_serial_id(sensor);

    sgp30_print_feature_set(&future_set);
    sgp30_print_serial_id(&serial_id);

    printf("SGP30 hat successfully initalized... \n \n");

    return;
}

void sgp30_iaq_init(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(SGP30_IAQ_INIT >> 8) & 0xff, (uint8_t)(SGP30_IAQ_INIT >> 0) & 0xff};

    sgp30_execute_command(sensor, writebuffer, 2, 10, NULL, 0);
}

sgp30_result_values_measure_iaq sgp30_measure_iaq(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(uint8_t)(SGP30_MEASURE_IAQ >> 8) & 0xff, (uint8_t)(SGP30_MEASURE_IAQ >> 0) & 0xff};
    uint8_t readbuffer[6] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 12, readbuffer, 6);

    uint16_t co2_result = (readbuffer[0] << 8) + readbuffer[1];
    uint16_t tvoc_result = (readbuffer[4] << 8) + readbuffer[3];
    sensor->eCO2 = co2_result;
    sensor->TVOC = tvoc_result;

    return (sgp30_result_values_measure_iaq){
        co2_result,
        readbuffer[2],
        tvoc_result,
        readbuffer[5],
    };
}

sgp30_result_values_iaq_baseline sgp30_get_iaq_baseline(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(uint8_t)(SGP30_GET_IAQ_BASELINE >> 8) & 0xff, (uint8_t)(SGP30_GET_IAQ_BASELINE >> 0) & 0xff};
    uint8_t readbuffer[6] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 10, readbuffer, 6);

    uint16_t co2_result = (readbuffer[0] << 8) + readbuffer[1];
    uint16_t tvoc_result = (readbuffer[4] << 8) + readbuffer[3];

    return (sgp30_result_values_iaq_baseline){
        co2_result,
        readbuffer[2],
        tvoc_result,
        readbuffer[5],
    };
}

void sgp30_set_absolute_humidity(sgp30_sensor *sensor, uint16_t absolute_humidity)
{
    uint8_t writebuffer[5] = {
        (SGP30_SET_ABSOLUTE_HUMIDITY >> 8) & 0xff,
        (uint8_t)(SGP30_SET_ABSOLUTE_HUMIDITY >> 0) & 0xff,
        (absolute_humidity >> (8 * 0)) & 0xff,
        (absolute_humidity >> (8 * 1)) & 0xff,
        sgp30_calculate_CRC((uint8_t *)&absolute_humidity, 2),
    };

    sgp30_execute_command(sensor, writebuffer, 5, 10, NULL, 0);
}

void sgp30_set_iaq_baseline(sgp30_sensor *sensor, uint16_t co2_baseline, uint16_t tvoc_baseline)
{

    uint8_t writebuffer[8] = {
        (SGP30_SET_IAQ_BASELINE >> 8) & 0xff,
        (uint8_t)(SGP30_SET_IAQ_BASELINE >> 0) & 0xff,
        (co2_baseline >> (8 * 0)) & 0xff,
        (co2_baseline >> (8 * 1)) & 0xff,
        sgp30_calculate_CRC((uint8_t *)&co2_baseline, 2),
        (tvoc_baseline >> (8 * 0)) & 0xff,
        (tvoc_baseline >> (8 * 1)) & 0xff,
        sgp30_calculate_CRC((uint8_t *)&tvoc_baseline, 2),
    };

    sgp30_execute_command(sensor, writebuffer, 8, 10, NULL, 0);
}

sgp30_result_get_serial_id sgp30_get_serial_id(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(SGP30_GET_SERIAL_ID >> 8) & 0xff, (uint8_t)(SGP30_GET_SERIAL_ID >> 0) & 0xff};
    uint8_t readbuffer[9] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 1000, readbuffer, 9);

    uint16_t first_id_word = (readbuffer[0] << 8) + readbuffer[1];
    uint16_t second_id_word = (readbuffer[3] << 8) + readbuffer[4];
    uint16_t third_id_word = (readbuffer[6] << 8) + readbuffer[7];
    uint16_t serial_id[3] = {first_id_word,
                             second_id_word,
                             third_id_word};
    memcpy(sensor->serial_number, serial_id, 3 * 2);

    return (sgp30_result_get_serial_id){
        {first_id_word, second_id_word, third_id_word},
        readbuffer[2],
        readbuffer[5],
        readbuffer[9],
    };
}

sgp30_result_values_measure_test sgp30_measure_test(sgp30_sensor *sensor)
{

    uint8_t writebuffer[2] = {(SGP30_MEASURE_TEST >> 8) & 0xff, (uint8_t)(SGP30_MEASURE_TEST >> 0) & 0xff};
    uint8_t readbuffer[3] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 220, readbuffer, 3);

    uint16_t fixed_data_pattern = (readbuffer[0] << 8) + readbuffer[1];
    printf("%d;%d", readbuffer[0], readbuffer[1]);
    return (sgp30_result_values_measure_test){
        fixed_data_pattern,
        readbuffer[2],
    };
}

sgp30_result_get_feature_set sgp30_get_feature_set(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(SGP30_GET_FEATURE_SET >> 8) & 0xff, (uint8_t)(SGP30_GET_FEATURE_SET >> 0) & 0xff};
    uint8_t readbuffer[3] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 10, readbuffer, 3);

    uint16_t feature_set = (readbuffer[0] << 8) + readbuffer[1];

    return (sgp30_result_get_feature_set){feature_set, readbuffer[3]};
}

sgp30_result_get_measure_raw sgp30_measure_raw(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(uint8_t)(SGP30_MEASURE_RAW >> 8) & 0xff, (uint8_t)(SGP30_MEASURE_RAW >> 0) & 0xff};
    uint8_t readbuffer[6] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 25, readbuffer, 6);

    uint16_t h2_result = (readbuffer[0] << 8) + readbuffer[1];
    uint16_t ethanol_result = (readbuffer[4] << 8) + readbuffer[3];

    sensor->raw_H2 = h2_result;
    sensor->raw_ethanol = ethanol_result;

    return (sgp30_result_get_measure_raw){
        h2_result,
        readbuffer[2],
        ethanol_result,
        readbuffer[5],
    };
}

sgp30_result_get_tvoc_inceptive_baseline sgp30_get_tvoc_inceptive_baseline(sgp30_sensor *sensor)
{
    uint8_t writebuffer[2] = {(SGP30_GET_TVOC_INCEPTICE_BASELINE >> 8) & 0xff, (uint8_t)(SGP30_GET_TVOC_INCEPTICE_BASELINE >> 0) & 0xff};
    uint8_t readbuffer[3] = {0};

    sgp30_execute_command(sensor, writebuffer, 2, 10, readbuffer, 3);

    uint16_t tvoc_baseline = (readbuffer[0] << 8) + readbuffer[1];
    return (sgp30_result_get_tvoc_inceptive_baseline){tvoc_baseline, readbuffer[3]};
}

void sgp30_set_tvoc_baseline(sgp30_sensor *sensor, uint16_t tvoc_baseline)
{
    uint8_t writebuffer[5] = {
        (SGP30_SET_TVOC_BASELINE >> 8) & 0xff,
        (uint8_t)(SGP30_SET_TVOC_BASELINE >> 0) & 0xff,
        (tvoc_baseline >> (8 * 0)) & 0xff,
        (tvoc_baseline >> (8 * 1)) & 0xff,
        sgp30_calculate_CRC((uint8_t *)&tvoc_baseline, 2),
    };

    sgp30_execute_command(sensor, writebuffer, 5, 10, NULL, 0);
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

void sgp30_print_measure_iaq(sgp30_result_values_measure_iaq *results)
{

    printf("Results for measure_iaq:\n     The CO2 Data is: %d;\n     The CRC of CO2 is: %d;\n     The Tvoc Data is: %d;\n     The CRC of Tvoc is: %d;\n", results->co2_result, results->crc_co2, results->tvoc_result, results->crc_tvoc);
}

void sgp30_print_measure_test(sgp30_result_values_measure_test *results)
{

    printf("Results for measure_test:\n     The fixed Data Pattern is: %d;\n     The CRC is: %d;\n", results->fixed_data_pattern, results->crc);
}

void sgp30_print_serial_id(sgp30_result_get_serial_id *results)
{

    printf("Results for the serial id:\n     The Serial ID is: %04x %04x %04x;\n", results->serial_id[0], results->serial_id[1], results->serial_id[2]);
}

void sgp30_print_feature_set(sgp30_result_get_feature_set *results)
{

    printf("Results for feature set:\n     The Feature Set is: %04x;\n", results->future_set);
}
