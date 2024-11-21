#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "esp_wifi.h"
#include "esp_log.h"


#include "defines.h"


int fake_function(void);

// Function to calculate checksum (simple XOR)
uint8_t calculate_checksum(const uint8_t *data, size_t len);

// Function to process received packet
void process_packet(const uint8_t *packet, size_t len);

#endif // PROTOCOL_H