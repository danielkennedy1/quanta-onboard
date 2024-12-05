#ifndef AIR_TEMP_H
#define AIR_TEMP_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define SIM_AIR_INITIAL_TEMP 25.0 // Initial air temperature (Celsius)
#define SIM_AIR_HEAT_CAPACITY 1.0 // Heat capacity of the air (J/C)
#define SIM_AIR_HEAT_TRANSFER 1 // Heat loss transfer rate (W)
#define SIM_AIR_TEMP_MIN 0.0 // Minimum air temperature (Celsius)
#define SIM_AIR_TEMP_MAX 100.0 // Maximum air temperature (Celsius)

#define SIM_TIME_INTERVAL 1 // Time interval (s)

#define HEATER_WATTAGE 10 // Heater wattage (W)
#define HEATER_ON true // Heater status

void sim_air_temp_task(void *pvParameters);

#endif // AIR_TEMP_H