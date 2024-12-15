#include "Controller.h"

#define TAG "Controller"

TaskHandle_t controller_task_handle;
esp_timer_handle_t timer;

bool waiting_for_command = true;

void IRAM_ATTR timerISR(void *arg) {
    vTaskNotifyGiveFromISR(controller_task_handle, NULL);
}

void init_timer(TaskHandle_t _controller_task_handle){
    ESP_LOGI(TAG, "Initializing timer");
    controller_task_handle = _controller_task_handle;

    esp_timer_create_args_t timer_args = {
        .callback = &timerISR,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_ISR,
        .name = "controller_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &timer));
};


void controller_task(void *pvParameters){
    
    ESP_LOGI(TAG, "Controller task running");
    QueueHandle_t command_queue = *(QueueHandle_t *)pvParameters;

    
    if (command_queue == NULL)
    {
        ESP_LOGE(TAG, "Command queue is NULL");
        vTaskDelete(NULL);
    }

    while (1)
    {
        if (!waiting_for_command){
            // wait for the timer to expire
            ESP_LOGI(TAG, "Waiting for timer to expire");
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }

        ESP_LOGI(TAG, "Waiting for command");
        waiting_for_command = true;

        Command command;

        // Default to shutting off the heater
        HeaterState heater_state = get_heater_state();
        heater_state.heater_on = false;
        ThermostatState thermostat_state = get_thermostat_state();
        thermostat_state.mode = THERMOSTAT_OFF;

        ESP_LOGI(TAG, "Shutting off heater and thermostat");
        set_heater_state(heater_state);
        set_thermostat_state(thermostat_state);

        // Wait for a command
        if (xQueueReceive(command_queue, &command, portMAX_DELAY) == pdTRUE)
        {
            ESP_LOGI(TAG, "Received command");
            waiting_for_command = false;

            switch (command.mode)
            {
            case SET_TARGET_TEMP_FOR_DURATION:
                // Give thermostat control of the heater, and set the target temperature
                ESP_LOGI(TAG, "Mode: SET_TARGET_TEMP_FOR_DURATION, Target Temp: %.2f, Duration: %d", command.data.target_temp, command.duration);
                thermostat_state.mode = THERMOSTAT_ON;
                thermostat_state.target_temp = command.data.target_temp;
                break;
            case SET_POWER_FOR_DURATION:
                // Turn the heater on or off directly for a duration
                ESP_LOGI(TAG, "Mode: SET_CONTROL_FOR_DURATION, Heater: %s, Duration: %d", command.data.heater_on ? "ON" : "OFF", command.duration);
                heater_state.heater_on = command.data.heater_on;
                break;
            }

            ESP_LOGW(TAG, "Resetting timer, duration: %d", command.duration);
            esp_timer_stop(timer);
            ESP_ERROR_CHECK(esp_timer_start_once(timer, command.duration * SECOND_IN_US));
        }
        set_heater_state(heater_state);
        set_thermostat_state(thermostat_state);
    }
}