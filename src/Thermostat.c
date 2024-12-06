#include "Thermostat.h"

#define TAG "Heater"

void thermostat_task(void *pvParameters){
    while (1)
    {
        HeaterState state = get_heater_state();
        switch (state.control_mode)
        {
        case SET_TARGET_TEMP:
            if (state.current_temp < state.target_temp)
            {
                state.heater_on = true;
            }
            else
            {
                state.heater_on = false;
            }
            break;
        case SET_CONTROL:
            break;
        }
        set_heater_state(state);
        ESP_LOGI(TAG, "Heater status: %s", state.heater_on ? "ON" : "OFF");
        vTaskDelay((HEATER_THERMOSTAT_INTERVAL * 1000) / portTICK_PERIOD_MS); 
    }
}