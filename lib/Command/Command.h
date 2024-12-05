#ifndef COMMAND_H
#define COMMAND_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "Protocol.h"
#include "SystemTime.h"

#define MAX_FUNCTIONS 16 // Theoretical maximum number of functions: 256

typedef Packet (*FunctionPointer)(const Packet *packet);

Packet heartbeat(const Packet *packet);
Packet system_time(const Packet *packet);

/*
    Function table
    Each function is associated with a function flag, which is used to identify the function to be called
*/
extern FunctionPointer protocol_function_table[];

// Process a received packet and return a response packet
Packet process_packet(const Packet *packet);

#endif // COMMAND_H