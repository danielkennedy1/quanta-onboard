#ifndef AGGREAGATOR_H
#define AGGREAGATOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "Heater.h"
#include "StateBuffer.h"

void aggregator_task(void *pvParameters);

#endif // AGGREAGATOR_H