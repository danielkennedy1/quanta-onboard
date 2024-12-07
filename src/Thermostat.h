#ifndef THERMOSTAT_H
#define THERMOSTAT_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "Heater.h"
#include "Command.h"

void thermostat_task(void *pvParameters);

#endif // THERMOSTAT_H