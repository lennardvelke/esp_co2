#include "esp32_now.h"
#include "esp_err.h"
#include "freertos/task.h"
#include "wifi.h"
#include <stdio.h>

void app_main()
{

        while (1)
    {
        vTaskDelay(100);
        // uint8_t data = {1, 2, 3, 4};
        // uint8_t mac[6] = {
        //     0xff,
        //     0xff,
        //     0xff,
        //     0xff,
        //     0xff,
        //     0xff,
        // };
        // ESP_ERROR_CHECK(esp_now_send(mac, &data, 4));
    }

    return;
}