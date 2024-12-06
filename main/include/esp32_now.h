#pragma once

#include "cc.h"
#include "esp_now.h"

#define BROADCAST_ADDRESS {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}

void esp32_now_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status);
void esp32_now_recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len);
void esp32_now_init(void (*fun_send_cb)(const uint8_t *mac_addr, esp_now_send_status_t status), void (*fun_recv_cb)(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len));
void esp32_now_init_with_callbacks();
