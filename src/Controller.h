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

// Initialize the timer, given the controller task handle
void init_timer(TaskHandle_t _controller_task_handle);

/*
 * Controller task
 * This task will: take a command from the command queue, execute the command, and then wait for the timer to expire
 * The timer will be used to trigger the controller task to check for a new command
 * If the command is to set the temperature, the controller will set the thermostat to be on and set the target temperature
 * If the command is to set the power, the controller will set the heater to be on or off
*/
void controller_task(void *pvParameters);

#endif // CONTROLLER_H