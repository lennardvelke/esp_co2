#pragma once

#include "device.h"
#include "esp_now.h"
#include <inttypes.h>

typedef struct
{
    uint8_t name[32];
    uint16_t send_intervall;
    void *get_funtion;
} slave_sensor;

/*
    To simpliefiy thing, we cap the limits of sensors per esp to 5
*/
static slave_sensor sensor_salve_list[5];
static uint8_t sensor_salve_list_length = 0;

void esp32_now_send_slave_cb(const uint8_t *mac_addr, esp_now_send_status_t status);

void esp32_now_recv_slave_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len);

uint8_t create_slave();

uint8_t add_sensor_slave(const char name[32], void (*function_pointer)());