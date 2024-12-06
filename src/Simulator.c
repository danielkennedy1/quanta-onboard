#include "Simulator.h"

#define TAG "Simulator"

float air_temp = SIM_AIR_INITIAL_TEMP; // Air temperature (Celsius)

void sim_air_temp_task(void *pvParameters) {
    while (1) {
        ESP_LOGI(TAG, "Simulator task running");
        HeaterState state = get_heater_state();
        // Step simulation

        float delta_joules = state.heater_on ? HEATER_WATTAGE : 0 - SIM_AIR_HEAT_TRANSFER;
        air_temp += delta_joules * SIM_TIME_INTERVAL / SIM_AIR_HEAT_CAPACITY;

        if (air_temp < SIM_AIR_TEMP_MIN) {
            air_temp = SIM_AIR_TEMP_MIN;
        } else if (air_temp > SIM_AIR_TEMP_MAX) {
            air_temp = SIM_AIR_TEMP_MAX;
        }

        ESP_LOGI(TAG, "Air temperature: %.2f", air_temp);

        state.current_temp = air_temp;
        set_heater_state(state);

        vTaskDelay((SIM_TIME_INTERVAL * 1000) / portTICK_PERIOD_MS);
    }
}