#include "defines.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP32 libraries
#include "esp_log.h"
#include "nvs_flash.h"

// Tasks
#include "TCP_Server.h"
#include "Simulator.h"
#include "Thermostat.h"

// Libraries
#include "Command.h"
#include "Wifi.h"
#include "SystemTime.h"
#include "Heater.h"

#define TAG "main"

#define WIFI false
#define SERVER false
#define SIM true

void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());


    if (WIFI) {
        initialize_wifi();
        initialize_system_time();
    }
    ESP_LOGI(TAG, "The current date/time is: %s", get_timestamp());

    if (SERVER) {
        xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
    }

    if (SIM) {
        init_heater_state();
        xTaskCreate(thermostat_task, "thermostat_task", 4096, NULL, 3, NULL);
        xTaskCreate(sim_air_temp_task, "sim_air_temp_task", 4096, NULL, 3, NULL);
    }


}
