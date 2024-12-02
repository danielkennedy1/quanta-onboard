#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "esp_wifi.h"
#include "esp_log.h"


#include "defines.h"

#define PACKET_SIZE 260
#define START_BYTE 0x02

// Protocol structure
/*
 * Total packet size: 260 bytes
 */
typedef struct {
    uint8_t start_byte;     // Start byte (byte 0)
    uint8_t function_flag;  // Function selector (byte 1)
    uint8_t payload_size;   // Size of the payload in bytes (byte 2)
    uint8_t payload[PACKET_SIZE - 3];   // Payload for function parameters (bytes 3 - 259)
} CommandPacket;

// Functions to convert CommandPacket to and from bytes
uint8_t* to_bytes(CommandPacket command_packet);
CommandPacket* from_bytes(uint8_t *bytes, size_t len);

// Function to process received packet
//void process_packet(const uint8_t *packet, size_t len);

#endif // PROTOCOL_H