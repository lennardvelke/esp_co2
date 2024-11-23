/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include "driver/i2c.h"
#include "driver/i2c_master.h"
#include "driver/i2c_types.h"
#include "esp_err.h"
#include "freertos/task.h"
#include "i2c_wrapper.h"
#include "sgp30.h"
#include <stdio.h>
#include <string.h>

#define I2C_MASTER_SCL_IO 22        /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 21        /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0            /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 400000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

uint16_t getAverage(uint16_t *array, uint16_t array_size)
{
    uint32_t AVG = 0;
    for (size_t i = 0; i < array_size - 1; i++)
    {
        AVG += array[i];
    }
    return AVG / array_size;
}

void app_main(void)
{
    sgp30_sensor sensor;

    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT, // clock source for LP i2c, might different from HP I2C
        .i2c_port = -1,                    // Assign to LP I2C port
        .scl_io_num = 22,                  // SCL IO number. Please refer to technical reference manual
        .sda_io_num = 21,                  // SDA IO number. Please refer to technical reference manual
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    i2c_master_bus_handle_t bus_handle;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = 0x58,
        .scl_speed_hz = 200000,
    };

    i2c_master_dev_handle_t dev_handle;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &dev_cfg, &dev_handle));

    esp_err_t x = i2c_master_probe(bus_handle, 0x58, -1);
    printf("Device connected: %d \n", x);

    sensor.dev_handle = &dev_handle;

    uint16_t last_five_min[300] = {0};

    uint16_t average = 0;

    // sgp30_init(&sensor);

    // sgp30_result_values_measure_test measure_test;

    // measure_test = sgp30_measure_test(&sensor);

    // sgp30_print_measure_test(&measure_test);

    sgp30_iaq_init(&sensor);
    // sgp30_set_iaq_baseline(&sensor, 48000, 38000);
    int n = 0;
    while (1)
    {
        vTaskDelay(300 / portTICK_PERIOD_MS);

        sgp30_result_values_measure_iaq temp = sgp30_measure_iaq(&sensor);
        sgp30_result_values_iaq_baseline temp2 = sgp30_get_iaq_baseline(&sensor);

        uint16_t last_item = last_five_min[299];

        memmove(&last_five_min[1], &last_five_min[0], (300 - 1) * sizeof(uint16_t));
        last_five_min[0] = temp.co2_result;

        average = getAverage(last_five_min, 300);

        printf("CO2: %d; AVG: %d; Baseline: %d \n", temp.co2_result, average, temp2.co2_result);
        // sgp30_print_measure_iaq(&temp);
    }

    sgp30_init(&sensor);

    return;
}