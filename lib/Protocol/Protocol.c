#include "Protocol.h"

uint8_t* to_bytes(CommandPacket command_packet) {
    uint8_t *bytes = malloc(260);
    if (bytes == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed");
        return NULL;
    }

    bytes[0] = command_packet.start_byte;
    bytes[1] = command_packet.function_flag;
    bytes[2] = command_packet.payload_size;
    for (size_t i = 0; i < command_packet.payload_size; i++) {
        bytes[3 + i] = command_packet.payload[i];
    }
    bytes[259] = command_packet.checksum;

    return bytes;
}

CommandPacket* from_bytes(uint8_t *bytes, size_t len) {
    CommandPacket* command_packet = malloc(sizeof(CommandPacket));
    if (len < 3) {
        ESP_LOGW(TAG, "Invalid bytes: too short");
        return NULL;
    }

    command_packet->start_byte = bytes[0];
    command_packet->function_flag = bytes[1];
    command_packet->payload_size = bytes[2];
    for (size_t i = 0; i < command_packet->payload_size; i++) {
        command_packet->payload[i] = bytes[3 + i];
    }
    command_packet->checksum = bytes[259];

    return command_packet;
}

uint8_t calculate_checksum(CommandPacket command_packet) {
    uint8_t checksum = 0;
    checksum ^= command_packet.start_byte;
    checksum ^= command_packet.function_flag;
    checksum ^= command_packet.payload_size;
    for (size_t i = 0; i < command_packet.payload_size; i++) {
        checksum ^= command_packet.payload[i];
    }
    return checksum;
}

bool verify_checksum(CommandPacket command_packet) {
    uint8_t checksum = calculate_checksum(command_packet);
    return checksum == command_packet.checksum;
}

/*
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
*/