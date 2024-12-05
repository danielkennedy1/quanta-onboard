#include "Simulator.h"

#define TAG "Simulator"

float air_temp = SIM_AIR_INITIAL_TEMP; // Air temperature (Celsius)

void sim_air_temp_task(void *pvParameters) {
    while (1) {
        // Step simulation
        float delta_temp = HEATER_ON ? HEATER_WATTAGE : 0 - SIM_AIR_HEAT_TRANSFER;
        air_temp += delta_temp * SIM_TIME_INTERVAL / SIM_AIR_HEAT_CAPACITY;

        if (air_temp < SIM_AIR_TEMP_MIN) {
            air_temp = SIM_AIR_TEMP_MIN;
        } else if (air_temp > SIM_AIR_TEMP_MAX) {
            air_temp = SIM_AIR_TEMP_MAX;
        }

        ESP_LOGI(TAG, "Air temperature: %.2f", air_temp);

        vTaskDelay((SIM_TIME_INTERVAL * 1000) / portTICK_PERIOD_MS);
    }
}