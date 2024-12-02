#include "defines.h"

// C standard libraries
#include <string.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP32 libraries
#include "esp_log.h"
#include "nvs_flash.h"

// Tasks
#include "TCP_Server.h"

// Libraries
#include "Command.h"
#include "Wifi.h"
#include "SystemTime.h"

#define TAG "main"

#define WIFI true
#define SERVER true

void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());

    if (WIFI) {
        initialize_wifi();
        initialize_system_time();
    }

    if (SERVER) {
        xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
    }

    ESP_LOGI(TAG, "The current date/time is: %s", get_timestamp());

}
