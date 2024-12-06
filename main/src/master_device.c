#include "master_device.h"
#include "esp32_now.h"
#include "esp_err.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "wifi.h"
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint8_t mac_addr[6];
} peer_slave;

static peer_slave peer_slave_list[20] = {0};
static peer_slave peer_slave_wait_for_connection_list[20] = {0};
static uint8_t peer_slave_length = 0;
static uint8_t connect_peer_slave_length = 0;

void esp32_now_send_master_cb(const uint8_t *mac_addr, esp_now_send_status_t status)
{

    return;
}

void esp32_now_recv_master_cb(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len)
{
    uint8_t broadcast_address[6] = BROADCAST_ADDRESS;

    if (esp_now_info->des_addr == broadcast_address)
    {
        printf("New Device is trying to connect... Mac: " MACSTR "!", MAC2STR(esp_now_info->src_addr));
        esp_now_peer_info_t unicast_peer = {
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
        ESP_ERROR_CHECK(esp_now_add_peer(&unicast_peer));

        uint8_t *master_mac_address = {0};

        ESP_ERROR_CHECK(esp_wifi_get_mac(DEFAULT_WIFI_IFIDX, master_mac_address));

        ESP_ERROR_CHECK(esp_now_send(unicast_peer.peer_addr, master_mac_address, 6));

        peer_slave current_recv = {
            {
                unicast_peer.peer_addr[0],
                unicast_peer.peer_addr[1],
                unicast_peer.peer_addr[2],
                unicast_peer.peer_addr[3],
                unicast_peer.peer_addr[4],
                unicast_peer.peer_addr[5],
            },
        };
        peer_slave_wait_for_connection_list[connect_peer_slave_length] = current_recv;
        connect_peer_slave_length++;
    }
    else
    {
        peer_slave current_recv = {
            {MAC2STR(esp_now_info->src_addr)},
        };
        for (size_t count = 0; count < connect_peer_slave_length; count++)
        {
            if (memcmp(&current_recv, &peer_slave_wait_for_connection_list[count], 6) == 0)
            {
                peer_slave_list[peer_slave_length] = current_recv;
                peer_slave_length++;
                break;

                /*
                    Maybe asking for Data
                 */

                return;
                printf("Connection was created between Master and " MACSTR "!\n", MAC2STR(esp_now_info->src_addr));
            }
        }

        printf("Recieved Data from a known Device!");
    }

    return;
}

uint8_t create_master(const char name[32], void (*function_pointer)())
{
    wifi_init_including_nvs();

    esp32_now_init(&esp32_now_send_cb, &esp32_now_recv_cb);

    return 0;
}
