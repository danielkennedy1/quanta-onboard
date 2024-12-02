#include "defines.h"

// C standard libraries
#include <stdio.h>
#include <string.h>
#include <time.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

// ESP32 libraries
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_sntp.h"

// Tasks
#include "TCP_Server.h"

// Libraries
#include "Command.h"
#include "Wifi.h"

#define TAG "main"

#define WIFI true
#define SERVER false


void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Time synchronization event received");
}

void obtain_time(void) {
    ESP_LOGI(TAG, "Initializing SNTP");

    // Initialize the SNTP service
    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "pool.ntp.org"); // Use a default NTP server
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();

    // Wait for time to be set
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait 2 seconds
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGE(TAG, "Failed to synchronize time");
    } else {
        ESP_LOGI(TAG, "Time synchronized successfully");
    }
}


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


    obtain_time();

    // Display synchronized time
    time_t now;
    struct tm timestamp;
    char strftime_buf[64];

    time(&now);
    localtime_r(&now, &timestamp);

    // Format and print the time
    strftime(strftime_buf, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S.000%Z", &timestamp);
    ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);

}
