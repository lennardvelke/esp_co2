#ifndef __SGP30_H__
#define __SGP30_H__

#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "hal/i2c_types.h"
#include <inttypes.h>

#define SGP30_FEATURESET 0x0020    /**< The required set for this library */
#define SGP30_CRC8_POLYNOMIAL 0x31 /**< Seed for SGP30's CRC polynomial */
#define SGP30_CRC8_INIT 0xFF       /**< Init value for CRC */
#define SGP30_WORD_LEN 2           /**< 2 bytes per word */

//!!!
#define NULL_REG 0xFF

typedef enum uint16_t
{
    SGP30_IAQ_INIT = 0x2003,
    SGP30_MEASURE_IAQ = 0x2008,
    SGP30_GET_IAQ_BASELINE = 0x2015,
    SGP30_SET_IAQ_BASELINE = 0x201e,
    SGP30_SET_ABSOLUTE_HUMIDITY = 0x2061,
    SGP30_MEASURE_TEST = 0x2032,
    SGP30_GET_FEATURE_SET = 0x202f,
    SGP30_MEASURE_RAW = 0x2050,
    SGP30_GET_TVOC_INCEPTICE_BASELINE = 0x20b3,
    SGP30_SET_TVOC_BASELINE = 0x2077,
    SGP30_GET_SERIAL_ID = 0x3682,
} spg30_command;

typedef struct
{
    /**< The 48-bit serial number, this value is set when you call sgp30_init */
    uint16_t serial_number[3];

    /**< The last measurement of the IAQ-calculated Total Volatile Organic
            Compounds in ppb. This value is set when you call IAQmeasure() **/
    uint16_t TVOC;

    /**< The last measurement of the IAQ-calculated equivalent CO2 in ppm. This
            value is set when you call IAQmeasure() */
    uint16_t eCO2;

    /**< The last measurement of the IAQ-calculated equivalent CO2 in ppm. This
            value is set when you call IAQmeasureRaw() */
    uint16_t raw_H2;

    /**< The last measurement of the IAQ-calculated equivalent CO2 in ppm. This
            value is set when you call IAQmeasureRaw */
    uint16_t raw_ethanol;

    /**< Interface pointer, used to store I2C address of the device */
    void *intf_ptr;

    i2c_master_dev_handle_t *dev_handle;
} sgp30_sensor;

typedef struct
{
    uint16_t fixed_data_pattern;
    uint8_t crc;
} sgp30_return_values_measure_test;

typedef struct
{
    uint16_t co2_result;
    uint8_t crc_co2;
    uint16_t tvoc_result;
    uint8_t crc_tvoc;
} sgp30_return_values_measure_iaq;
typedef struct
{
    uint16_t co2_result;
    uint8_t crc_co2;
    uint16_t tvoc_result;
    uint8_t crc_tvoc;
} sgp30_return_values_iaq_baseline;

esp_err_t sgp30_execute_command(sgp30_sensor *sensor, uint8_t command[], uint8_t command_length, uint16_t delay, uint8_t *read_data, uint8_t read_data_length);

void sgp30_init(sgp30_sensor *sensor);

sgp30_return_values_measure_iaq sgp30_measure_iaq(sgp30_sensor *sensor);
sgp30_return_values_measure_test sgp30_measure_test(sgp30_sensor *sensor);
sgp30_return_values_iaq_baseline sgp30_get_iaq_baseline(sgp30_sensor *sensor);

void sgp30_iaq_init(sgp30_sensor *sensor);
void sgp30_set_iaq_baseline(sgp30_sensor *sensor);
void sgp30_set_absolute_humidity(sgp30_sensor *sensor);
void sgp30_set_iaq_baseline(sgp30_sensor *sensor);
void sgp30_get_feature_set(sgp30_sensor *sensor);
void sgp30_measure_raw(sgp30_sensor *sensor);
void sgp30_get_tvoc_inceptive_baseline(sgp30_sensor *sensor);
void sgp30_set_tvoc_baseline(sgp30_sensor *sensor);

uint8_t sgp30_calculate_CRC(uint8_t *data, uint8_t len);

void sgp30_print_measure_iaq(sgp30_return_values_measure_iaq *results);
void sgp30_print_measure_test(sgp30_return_values_measure_test *results);

#endif