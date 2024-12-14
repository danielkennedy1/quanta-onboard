#include "Controller.h"

#define TAG "Controller"

static QueueHandles queue_handles;

TaskHandle_t controller_task_handle;
volatile bool timer_flag = false;

esp_timer_handle_t timer;

void IRAM_ATTR timerISR(void *arg) {
    timer_flag = true;
}

void init_timer(TaskHandle_t _controller_task_handle){
    ESP_LOGI(TAG, "Initializing timer");
    controller_task_handle = _controller_task_handle;

    //ESP_ERROR_CHECK(esp_timer_init());

    esp_timer_create_args_t timer_args = {
        .callback = &timerISR,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "controller_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));

    ESP_ERROR_CHECK(esp_timer_start_once(timer, 1 * SECOND_IN_US));

    timer_flag = false;
};


void controller_task(void *pvParameters){
    
    queue_handles = *(QueueHandles *)pvParameters;

    
    if (queue_handles.command_queue == NULL)
    {
        ESP_LOGE(TAG, "Command queue is NULL");
        vTaskDelete(NULL);
    }

    while (1)
    {
        ESP_LOGI(TAG, "Controller task running");
        ESP_LOGI(TAG, "timer_flag: %d", timer_flag);

        if (!timer_flag)
        {
            ESP_LOGW(TAG, "Timer not expired, take reading or whatever");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "Waiting for command");

        Command command;

        if (xQueueReceive(queue_handles.command_queue, &command, 0) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received command");

            switch (command.mode)
            {
            case SET_TARGET_TEMP_FOR_DURATION:
                // Give thermostat control of the heater, and set the target temperature
                ESP_LOGI(TAG, "Mode: SET_TARGET_TEMP_FOR_DURATION, Target Temp: %.2f, Duration: %d", command.data.target_temp, command.duration);
                ThermostatState state = get_thermostat_state();
                state.target_temp = command.data.target_temp;
                state.mode = THERMOSTAT_ON;
                set_thermostat_state(state);
                break;
            case SET_POWER_FOR_DURATION:
                ESP_LOGI(TAG, "Mode: SET_CONTROL_FOR_DURATION, Heater: %s, Duration: %d", command.data.heater_on ? "ON" : "OFF", command.duration);
                set_heater_state((HeaterState){.heater_on = command.data.heater_on});
                break;
            }

            ESP_LOGW(TAG, "Resetting timer, duration: %d", command.duration);
            esp_timer_stop(timer);
            timer_flag = false;
            ESP_ERROR_CHECK(esp_timer_start_once(timer, command.duration * SECOND_IN_US));
        } else {
            ESP_LOGI(TAG, "No command received");
            ESP_LOGI(TAG, "Shutting off heater");
            set_heater_state((HeaterState){.heater_on = false});
            set_thermostat_state((ThermostatState){.mode = THERMOSTAT_OFF});
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}