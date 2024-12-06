#ifndef HEATER_H
#define HEATER_H

#include "defines.h"

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t heater_state_mutex;

typedef enum {
    SET_POWER, // Set the heater to ON or OFF
    SET_TARGET_TEMP, // Set the target temperature
    WAIT // Wait for the next control command
} ControlMode;

typedef struct {
    bool heater_on;
    float target_temp;
    float current_temp;
    ControlMode control_mode;
} HeaterState;

void init_heater_state();
HeaterState get_heater_state();
void set_heater_state(HeaterState state);

#endif // HEATER_H