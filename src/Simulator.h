#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "Heater.h"
#include "Thermostat.h"


void sim_air_temp_task(void *pvParameters);

#endif // SIMULATOR_H