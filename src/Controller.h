#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "Command.h"
#include "Heater.h"

void controller_task(void *pvParameters);

#endif // CONTROLLER_H