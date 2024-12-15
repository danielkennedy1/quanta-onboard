#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#include "Heater.h"
#include "Command.h"

typedef enum {
    THERMOSTAT_ON,
    THERMOSTAT_OFF,
} ThermostatMode;

typedef struct {
    float target_temp;
    ThermostatMode mode;
} ThermostatState;

// Must be called before any other thermostat state functions
void init_thermostat_state();

// Thermostat state
ThermostatState get_thermostat_state();
void set_thermostat_state(ThermostatState state);

/*
    Thermostat task
    If the thermostat is on, the task will check the current temperature and adjust the heater state accordingly
*/
void thermostat_task(void *pvParameters);

#endif // THERMOSTAT_H