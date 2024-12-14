#ifndef HEATER_H
#define HEATER_H

#include "defines.h"

#include <stdbool.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t heater_state_mutex;

typedef struct {
    bool heater_on;
    float current_temp;
} HeaterState;

void init_heater_state();
HeaterState get_heater_state();
void set_heater_state(HeaterState state);

#endif // HEATER_H