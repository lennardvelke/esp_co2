#include "esp32_now.h"
#include "esp_now.h"
#include "wifi.h"
void esp32_now_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{

    printf("The send callback was called with the mac-Address " MACSTR "! The Status is %d \n", MAC2STR(mac_addr), status);
}

void esp32_now_recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{
}

void esp32_now_init()
{
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(esp32_now_recv_cb));
    ESP_ERROR_CHECK(esp_now_register_send_cb(esp32_now_send_cb));

    esp_now_peer_info_t broadcast_peer = {
        .peer_addr = BROADCAST_ADDRESS,
        .channel = 0,
        .ifidx = DEFAULT_WIFI_IFIDX,
        .encrypt = false,

    };

    ESP_ERROR_CHECK(esp_now_add_peer(&broadcast_peer));

    uint8_t data = {1, 2, 3, 4};

    ESP_ERROR_CHECK(esp_now_send(broadcast_peer.peer_addr, &data, 4));
}
