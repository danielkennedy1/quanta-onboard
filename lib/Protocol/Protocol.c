#include "Protocol.h"

#define TAG "Protocol"

uint8_t* to_bytes(Packet command_packet) {
    uint8_t *bytes = malloc(PACKET_SIZE);
    if (bytes == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed");
        return NULL;
    }
    memset(bytes, 0, PACKET_SIZE);

    bytes[0] = command_packet.start_byte;
    bytes[1] = command_packet.function_flag;
    bytes[2] = command_packet.payload_size;
    for (size_t i = 0; i < command_packet.payload_size; i++) {
        bytes[3 + i] = command_packet.payload[i];
    }

    return bytes;
}

Packet* from_bytes(uint8_t *bytes, size_t len) {
    Packet* packet = malloc(sizeof(Packet));
    if (len < 3) {
        ESP_LOGW(TAG, "Invalid bytes: too short");
        return NULL;
    }

    packet->start_byte = bytes[0];

    if (packet->start_byte != RX_START_BYTE) {
        ESP_LOGW(TAG, "Invalid bytes: missing start byte");
        free(packet);
        return NULL;
    }

    if (len < 3 + bytes[2]) {
        ESP_LOGW(TAG, "Invalid bytes: length mismatch");
        free(packet);
        return NULL;
    }

    packet->function_flag = bytes[1];
    packet->payload_size = bytes[2];

    if (packet->payload_size > PACKET_SIZE - 3) {
        ESP_LOGE(TAG, "Payload is too big!");
        free(packet);
        return NULL;
    }

    for (size_t i = 0; i < packet->payload_size; i++) {
        packet->payload[i] = bytes[3 + i];
    }
    return packet;
}