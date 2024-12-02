#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "Protocol.h"

#define MAX_FUNCTIONS 16 // Theoretical maximum number of functions: 256

typedef CommandPacket (*FunctionPointer)(const CommandPacket *packet);

CommandPacket heartbeat(const CommandPacket *packet);

/*
    Function table
    Each function is associated with a function flag, which is used to identify the function to be called
*/
extern FunctionPointer protocol_function_table[];

// Process a received packet and return a response packet
CommandPacket process_packet(const CommandPacket *packet);

#endif // COMMAND_H