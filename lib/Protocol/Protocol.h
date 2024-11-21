#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "esp_wifi.h"


int fake_function(void);

// Function to calculate checksum (simple XOR)
uint8_t calculate_checksum(const uint8_t *data, size_t len);

#endif // PROTOCOL_H