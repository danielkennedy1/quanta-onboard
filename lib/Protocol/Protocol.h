#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#include "defines.h"

#define PACKET_SIZE 259

// Start byte for received packets (could be used as version number)
#define RX_START_BYTE 0x02

// Start byte for transmitted packets
#define TX_START_BYTE 0x03

// Protocol structure
/*
 * A unint8_t has a range of 0 to 255, so the maximum size of the payload is 255 bytes
 */
typedef struct {
    uint8_t start_byte;             // Start byte (byte 0)
    uint8_t function_flag;          // Function selector (byte 1)
    uint8_t payload_size;           // Size of the payload in bytes (byte 2)
    uint8_t payload[UINT8_MAX];     // Payload for function parameters (bytes 3 - 259)
} Packet;

// Function to convert CommandPacket to bytes
uint8_t* to_bytes(Packet command_packet);

// Function to convert bytes to CommandPacket (WARNING: MEMORY ALLOCATED FOR RETURNED PACKET POINTER)
Packet* from_bytes(uint8_t *bytes, size_t len);

#endif // PROTOCOL_H