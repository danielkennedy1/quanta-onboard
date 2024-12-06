#ifndef HEATER_H
#define HEATER_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "State.h"

void heater_task(void *pvParameters);


#endif // HEATER_H