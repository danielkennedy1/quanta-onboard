#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "defines.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "soc/timer_group_struct.h"

#include "Command.h"
#include "Thermostat.h"
#include "Heater.h"

#define SECOND_IN_US   1000000       // 1 second in microseconds

void init_timer(TaskHandle_t _controller_task_handle);

void controller_task(void *pvParameters);

#endif // CONTROLLER_H