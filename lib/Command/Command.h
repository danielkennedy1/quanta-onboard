#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "Protocol.h"
#include "SystemTime.h"

typedef enum {
    SET_POWER_FOR_DURATION, // Set the heater to ON or OFF
    SET_TARGET_TEMP_FOR_DURATION, // Set the target temperature
} CommandType;

typedef union {
    bool heater_on;
    float target_temp;
} CommandData;

typedef struct {
    CommandType mode;
    CommandData data;
    int duration; // Duration of the command (in seconds)
} Command;

typedef struct {
    QueueHandle_t command_queue;
    QueueHandle_t air_temp_queue;
} QueueHandles;

typedef Packet (*FunctionPointer)(const Packet *packet);

Packet heartbeat(const Packet *packet);
Packet get_system_time(const Packet *packet);
Packet set_temperature_for_duration(const Packet *packet);
Packet set_control_for_duration(const Packet *packet);

/*
    Function table
    Each function is associated with a function flag, which is used to identify the function to be called
*/
extern FunctionPointer protocol_function_table[];

// Process a received packet and return a response packet
Packet process_packet(const Packet *packet);

#endif // COMMAND_H