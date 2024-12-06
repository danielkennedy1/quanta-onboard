#ifndef AIR_TEMP_H
#define AIR_TEMP_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "State.h"
#include "Heater.h"


void sim_air_temp_task(void *pvParameters);

#endif // AIR_TEMP_H