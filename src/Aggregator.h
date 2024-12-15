#ifndef AGGREAGATOR_H
#define AGGREAGATOR_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "Heater.h"
#include "StateBuffer.h"

/*
 * Aggregator task
 * This task will run at a predefined interval and aggregate the data in the state buffer
*/
void aggregator_task(void *pvParameters);

#endif // AGGREAGATOR_H