#pragma once

#include "esp_now.h"
#include <inttypes.h>

typedef struct
{
    uint8_t mac[6];
    uint8_t name[32];

} slave_device;

void esp32_now_send_master_cb(const uint8_t *mac_addr, esp_now_send_status_t status);

void esp32_now_recv_master_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len);

uint8_t create_master(const char name[32], void (*function_pointer)());