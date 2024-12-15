#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "Heater.h"
#include "Thermostat.h"

/*
    Simulator task
    This task will simulate the air temperature in the room
    Simple model: energy in/out is linear
    Reads heater state and mutates air temperature
*/
void sim_air_temp_task(void *pvParameters);

#endif // SIMULATOR_H