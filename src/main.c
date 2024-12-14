#include "defines.h"

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// ESP32
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

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

#define KILLSWITCH_PIN GPIO_NUM_9
SemaphoreHandle_t killswitch_semaphore;

#define WIFI true

// ISR handler
static void IRAM_ATTR killswitch_isr_handler(void* arg) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Give the semaphore from the ISR
    xSemaphoreGiveFromISR(killswitch_semaphore, &xHigherPriorityTaskWoken);

    // Yield to a higher-priority task if needed
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}

void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());

    QueueHandles queue_handles = {
        .command_queue = xQueueCreate(10, sizeof(Command)),
        .air_temp_queue = xQueueCreate(10, sizeof(float))
    };

    TaskHandle_t controller_task_handle;
    TaskHandle_t thermostat_task_handle;

    if (WIFI) {
        initialize_wifi();
        initialize_system_time();
        xTaskCreate(tcp_server_task, "tcp_server_task", 4096, &queue_handles, 5, NULL);
    }

    ESP_LOGI(TAG, "The current date/time is: %s", get_timestamp());

    init_heater_state();
    init_thermostat_state();
    xTaskCreate(thermostat_task, "thermostat_task", 4096, NULL, 3, &thermostat_task_handle);

    xTaskCreate(controller_task, "controller_task", 4096, &queue_handles, 3, &controller_task_handle);
    init_timer(controller_task_handle);


    xTaskCreate(sim_air_temp_task, "sim_air_temp_task", 4096, NULL, 4, NULL);
    xTaskCreate(aggregator_task, "aggregator_task", 4096, NULL, 3, NULL);

    // Killswitch for the heater
    gpio_set_direction(KILLSWITCH_PIN, GPIO_MODE_INPUT);
    gpio_set_intr_type(KILLSWITCH_PIN, GPIO_INTR_NEGEDGE);
    gpio_pulldown_en(KILLSWITCH_PIN);
    gpio_pullup_dis(KILLSWITCH_PIN);

    killswitch_semaphore = xSemaphoreCreateBinary();

    gpio_install_isr_service(0);
    gpio_isr_handler_add(KILLSWITCH_PIN, killswitch_isr_handler, NULL);

    while (1) {
        if (xSemaphoreTake(killswitch_semaphore, portMAX_DELAY) == pdTRUE) {
            ESP_LOGE(TAG, "Killswitch activated! Turning off the heater...");
            set_heater_state((HeaterState) { .heater_on = false });

            vTaskDelete(controller_task_handle);
            vTaskDelete(thermostat_task_handle);
            vTaskDelete(NULL);
        }
    }
}