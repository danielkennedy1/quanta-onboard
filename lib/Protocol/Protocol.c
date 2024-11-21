#include "Protocol.h"


int fake_function(void) {
    return 12345;
}

uint8_t calculate_checksum(const uint8_t *data, size_t len) {
    uint8_t checksum = 0;
    for (size_t i = 0; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void process_packet(const uint8_t *packet, size_t len) {
    if (len < 3) {
        ESP_LOGW(TAG, "Invalid packet: too short");
        return;
    }

    // Check for start byte
    if (packet[0] != START_BYTE) {
        ESP_LOGW(TAG, "Invalid packet: missing start byte");
        return;
    }

    // Extract length and payload
    uint8_t length = packet[1];
    if (length + 3 > len) {
        ESP_LOGW(TAG, "Invalid packet: length mismatch");
        return;
    }

    const uint8_t *payload = &packet[2];
    uint8_t checksum = packet[2 + length];

    // Verify checksum
    if (calculate_checksum(payload, length) != checksum) {
        ESP_LOGW(TAG, "Invalid packet: checksum mismatch");
        return;
    }

    // Handle payload
    ESP_LOGI(TAG, "Valid packet received: Payload (length=%d)", length);
       // Check for null pointer or zero length
    if (payload == NULL || length == 0) {
        ESP_LOGW(TAG, "Payload is empty");
        return;
    }

    // Allocate a buffer for the ASCII string (add 1 for null-terminator)
    char *ascii_str = malloc(length + 1);
    if (ascii_str == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed");
        return;
    }

    // Convert payload to ASCII characters
    for (size_t i = 0; i < length; i++) {
        // Check if byte is printable ASCII, otherwise replace with '.'
        ascii_str[i] = (payload[i] >= 32 && payload[i] <= 126) ? (char)payload[i] : '.';
    }
    ascii_str[length] = '\0';  // Null-terminate the string

    // Log the ASCII string
    ESP_LOGI(TAG, "Payload as ASCII: %s", ascii_str);
}
