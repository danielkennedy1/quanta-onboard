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
#include "Controller.h"
#include "Aggregator.h"

// Libraries
#include "Command.h"
#include "Wifi.h"
#include "SystemTime.h"
#include "Heater.h"

#define TAG "main"

#define WIFI true
#define SERVER true
#define SIM true

void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());

    QueueHandles queue_handles = {
        .command_queue = xQueueCreate(10, sizeof(Command)),
        .air_temp_queue = xQueueCreate(10, sizeof(float))
    };

    if (WIFI) {
        initialize_wifi();
        initialize_system_time();
    }
    ESP_LOGI(TAG, "The current date/time is: %s", get_timestamp());

    if (SERVER) {
        xTaskCreate(tcp_server_task, "tcp_server_task", 4096, &queue_handles, 5, NULL);
    }

    if (SIM) {


        init_heater_state();
        init_thermostat_state();
        xTaskCreate(thermostat_task, "thermostat_task", 4096, &queue_handles, 3, NULL);

        TaskHandle_t controller_task_handle;
        xTaskCreate(controller_task, "controller_task", 4096, &queue_handles, 3, &controller_task_handle);
        init_timer(controller_task_handle);


        xTaskCreate(sim_air_temp_task, "sim_air_temp_task", 4096, NULL, 3, NULL);

        xTaskCreate(aggregator_task, "aggregator_task", 4096, NULL, 3, NULL);

        vTaskDelay(1000 / portTICK_PERIOD_MS);

        Command command = {
            .mode = SET_TARGET_TEMP_FOR_DURATION,
            .data.target_temp = 30.0,
            .duration = 10
        };

        ESP_LOGW(TAG, "Sending 1st command");
        xQueueSend(queue_handles.command_queue, &command, 0);
        command.data.target_temp = 35.0;

        ESP_LOGW(TAG, "Sending 2nd command");
        xQueueSend(queue_handles.command_queue, &command, 0);

        while (1) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}
