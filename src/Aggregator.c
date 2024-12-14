#include "Aggregator.h"

#define TAG "Aggregator"

void aggregator_task(void *pvParameters) {
    ESP_LOGI(TAG, "Aggregator task started");

    // TODO: Wait here for system time to be set (cb notifies this task)

    init_state_buffer(time(NULL));

    while (1) {
        ESP_LOGI(TAG, "Aggregator task running");

        HeaterState state = get_heater_state();
        add_air_temp(state.current_temp);
        add_power_state(state.heater_on);

        vTaskDelay((1000 / AGG_SAMPLES_PER_SECOND) / portTICK_PERIOD_MS);
    }
}