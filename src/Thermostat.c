#include "Thermostat.h"

#define TAG "Thermostat"

void thermostat_task(void *pvParameters){
    while (1)
    {   
        ESP_LOGI(TAG, "Thermostat task running");

        HeaterState state = get_heater_state();

        switch (state.control_mode)
        {
        case SET_TARGET_TEMP:
            ESP_LOGI(TAG, "Mode: SET_TARGET_TEMP, Target Temp: %.2f", state.target_temp);
            if (state.current_temp < state.target_temp)
            {
                state.heater_on = true;
            }
            else
            {
                state.heater_on = false;
            }
            break;
        case SET_POWER:
            ESP_LOGI(TAG, "Mode: SET_CONTROL, Heater: %s", state.heater_on ? "ON" : "OFF");
            break;
        case WAIT:
            ESP_LOGI(TAG, "Mode: WAIT");
            state.heater_on = false;
            break;
        }
        set_heater_state(state);
        ESP_LOGI(TAG, "Heater power: %s", state.heater_on ? "ON" : "OFF");
        vTaskDelay((HEATER_THERMOSTAT_INTERVAL * 1000) / portTICK_PERIOD_MS);
    }
}