#pragma once

#include "esp_crc.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_random.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

#define DEFAULT_WIFI_MODE WIFI_MODE_STA
#define DEFAULT_WIFI_CHANNEL 1
#define DEFAULT_WIFI_IFIDX WIFI_IF_STA
#define DEFAULT_ESP_NOW_IFIDX DEFAULT_WIFI_IFIDX

void wifi_init_including_nvs();
void wifi_init();
