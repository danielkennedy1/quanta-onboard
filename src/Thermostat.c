#include "Thermostat.h"

#define TAG "Thermostat"

SemaphoreHandle_t thermostat_state_mutex;

static ThermostatState thermostat_state;

void init_thermostat_state(){
    thermostat_state_mutex = xSemaphoreCreateMutex();
    thermostat_state.mode = THERMOSTAT_OFF;
    thermostat_state.target_temp = 0.0;
}

ThermostatState get_thermostat_state(){
    xSemaphoreTake(thermostat_state_mutex, portMAX_DELAY);
    ThermostatState state = thermostat_state;
    xSemaphoreGive(thermostat_state_mutex);
    return state;
}

void set_thermostat_state(ThermostatState state){
    xSemaphoreTake(thermostat_state_mutex, portMAX_DELAY);
    thermostat_state = state;
    xSemaphoreGive(thermostat_state_mutex);
}

void thermostat_task(void *pvParameters){
    while (1)
    {   
        ESP_LOGI(TAG, "Thermostat task running");

        HeaterState heater_state = get_heater_state();
        ThermostatState thermostat_state = get_thermostat_state();

        switch (thermostat_state.mode)
        {
        case THERMOSTAT_ON:
            ESP_LOGI(TAG, "Thermostat is ON");
            if (heater_state.current_temp < thermostat_state.target_temp){
                heater_state.heater_on = true;
            } else {
                heater_state.heater_on = false;
            }
            break;
        case THERMOSTAT_OFF:
            ESP_LOGI(TAG, "Thermostat is OFF");
            break;
        }
        set_heater_state(heater_state);
        ESP_LOGI(TAG, "Heater power: %s", heater_state.heater_on ? "ON" : "OFF");
        vTaskDelay((HEATER_THERMOSTAT_INTERVAL * 1000) / portTICK_PERIOD_MS);
    }
}