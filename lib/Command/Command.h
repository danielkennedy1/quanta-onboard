#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "Protocol.h"
#include "SystemTime.h"
#include "StateBuffer.h"

/* 
    Command structure
    Command
    - mode: CommandType (SET_POWER_FOR_DURATION, SET_TARGET_TEMP_FOR_DURATION) tells the controller what to do
    - data: CommandData (heater_on, target_temp) data associated with the command
    - duration: int duration of the command (in seconds), how long the command should be executed for
    - written: bool flag to indicate if the command has been written (used by server, if written the command is queued)
*/
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
    bool written; // Flag to indicate if the command has been written
} Command;

typedef Packet (*FunctionPointer)(const Packet *packet, Command *command);

Packet heartbeat(const Packet *packet, Command *command);
Packet get_system_time(const Packet *packet, Command *command);
Packet set_temperature_for_duration(const Packet *packet, Command *command);
Packet set_control_for_duration(const Packet *packet, Command *command);
Packet get_temperature_for_minute(const Packet *packet, Command *command);
Packet get_uptime_for_minute(const Packet *packet, Command *command);

/*
    Function table
    Each function is associated with a function flag, which is used to identify the function to be called
*/
extern FunctionPointer protocol_function_table[];

// Process a received packet, write a command and return a response packet
Packet process_packet(const Packet *packet, Command *command);

#endif // COMMAND_H