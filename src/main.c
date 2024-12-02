#include "defines.h"

// C standard libraries
#include <stdio.h>
#include <string.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

// ESP32 libraries
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Tasks
#include "TCP_Server.h"

// Libraries
#include "Command.h"
#include "Wifi.h"

#define TAG "main"

#define WIFI false
#define SERVER false

void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());

    if (WIFI) {
        // Initialize Wi-Fi
        wifi_event_handler_context_t wifi_event_handler_context = {
            .wifi_event_group = xEventGroupCreate(),
            .wifi_connected_bit = BIT0,
            .retry_count = 0,
        };
        wifi_init_sta(&wifi_event_handler_context);

        // Wait for Wi-Fi connection
        ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");
        xEventGroupWaitBits(wifi_event_handler_context.wifi_event_group, wifi_event_handler_context.wifi_connected_bit, pdFALSE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Connected to Wi-Fi");
    }

    if (SERVER) {
        xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
    }

    
}
