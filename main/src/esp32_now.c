#include "esp32_now.h"
#include "esp_now.h"
void esp32_now_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{
}

void esp32_now_recv_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{
}

void esp32_now_init()
{
    esp_now_init();
    esp_now_register_recv_cb(esp32_now_recv_cb);
    esp_now_register_send_cb(esp32_now_send_cb);
}
