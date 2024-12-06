#include "State.h"

SemaphoreHandle_t heater_state_mutex;
static HeaterState heater_state;

void init_heater_state() {
    heater_state_mutex = xSemaphoreCreateMutex();
    heater_state.heater_on = STATE_INITIAL_ON;
    heater_state.target_temp = STATE_INITIAL_TARGET_TEMP;
    heater_state.current_temp = SIM_AIR_INITIAL_TEMP;
    heater_state.control_mode = STATE_INITIAL_CONTROL_MODE;
}

HeaterState get_heater_state() {
    xSemaphoreTake(heater_state_mutex, portMAX_DELAY);
    HeaterState state = heater_state;
    xSemaphoreGive(heater_state_mutex);
    return state;
}

void set_heater_state(HeaterState state) {
    xSemaphoreTake(heater_state_mutex, portMAX_DELAY);
    heater_state = state;
    xSemaphoreGive(heater_state_mutex);
}