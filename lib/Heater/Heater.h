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

// Must be called before any other heater state functions
void init_heater_state();

// Get the current heater state
HeaterState get_heater_state();

// Set the heater state
void set_heater_state(HeaterState state);

#endif // HEATER_H