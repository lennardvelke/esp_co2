#include "slave_device.h"

#include "esp32_now.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "master_device.h"
#include "wifi.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint8_t mac_addr[6];
} peer_master;

static peer_master peer_slave_list = {0};

void esp32_now_send_master_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{

    printf("Any Sensor send some data!");
    return;
}

void esp32_now_recv_master_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{

    printf("The entire Device recieved some data!");
    esp_now_peer_info_t master_peer = {
        .peer_addr = {
            esp_now_info->src_addr[0],
            esp_now_info->src_addr[1],
            esp_now_info->src_addr[2],
            esp_now_info->src_addr[3],
            esp_now_info->src_addr[4],
            esp_now_info->src_addr[5]},
        .channel = 0,
        .ifidx = DEFAULT_WIFI_IFIDX,
        .encrypt = false,
    };
    ESP_ERROR_CHECK(esp_now_add_peer(&master_peer));
    const uint8_t *data_send = {4, 2, 3, 4};
    ESP_ERROR_CHECK(esp_now_send(master_peer.peer_addr, data_send, 4));
    return;
}

uint8_t create_slave()
{
    wifi_init_including_nvs();

    esp32_now_init(&esp32_now_send_cb, &esp32_now_recv_cb);

    return 0;
}
uint8_t add_sensor_slave(const char name[32], void (*function_pointer)())
{
    memcpy(&sensor_salve_list[sensor_salve_list_length], name, 32);
    sensor_salve_list->send_intervall = 5000;
    sensor_salve_list->get_funtion = function_pointer;

    return 0;
}
