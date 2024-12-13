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
        } else {
            ESP_LOGW(TAG, "No command received");
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}