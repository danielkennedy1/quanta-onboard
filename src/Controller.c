#include "Controller.h"

#define TAG "Controller"

static QueueHandles queue_handles;

void controller_task(void *pvParameters){
    
    queue_handles = *(QueueHandles *)pvParameters;

    while (1)
    {
        ESP_LOGI(TAG, "Controller task running");
        ESP_LOGI(TAG, "Waiting for command");

        if (queue_handles.command_queue == NULL)
        {
            ESP_LOGE(TAG, "Command queue is NULL");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        Command command;

        if (xQueueReceive(queue_handles.command_queue, &command, 0) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received command");
            HeaterState state = get_heater_state();

            switch (command.mode)
            {
            case SET_TARGET_TEMP_FOR_DURATION:
                ESP_LOGI(TAG, "Mode: SET_TARGET_TEMP_FOR_DURATION, Target Temp: %.2f, Duration: %d", command.data.target_temp, command.duration);
                state.target_temp = command.data.target_temp;
                state.control_mode = SET_TARGET_TEMP;
                break;
            case SET_POWER_FOR_DURATION:
                ESP_LOGI(TAG, "Mode: SET_CONTROL_FOR_DURATION, Heater: %s, Duration: %d", command.data.heater_on ? "ON" : "OFF", command.duration);
                state.heater_on = command.data.heater_on;
                state.control_mode = SET_POWER;
                break;
            }

            set_heater_state(state);
        } else {
            ESP_LOGW(TAG, "No command received");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}