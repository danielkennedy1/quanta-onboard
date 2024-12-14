#ifndef STATEBUFFER_H
#define STATEBUFFER_H

#include <stdlib.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "esp_log.h"

#define SECONDS_IN_MINUTE 60
#define MINUTES_TO_STORE 10

#define BUFFER_SIZE (MINUTES_TO_STORE * SECONDS_IN_MINUTE)

typedef struct {
    bool buffer[BUFFER_SIZE];
    int read_head;
    int write_head;
    bool is_empty;
} BoolBuffer;

typedef struct {
    float buffer[BUFFER_SIZE];
    int read_head;
    int write_head;
    bool is_empty;
} FloatBuffer;

void init_state_buffer(time_t current_time);

void add_air_temp(float air_temp);
bool get_next_minute_average_air_temp(float* average_air_temp, time_t* timestamp);

void add_power_state(bool power_state);
bool get_next_minute_uptime(float* uptime, time_t* timestamp);

#endif // STATEBUFFER_H