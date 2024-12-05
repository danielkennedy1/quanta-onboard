#include "Command.h"

#define TAG "Command"

FunctionPointer protocol_function_table[MAX_FUNCTIONS] = {
    [0x00] = heartbeat,
    [0x01] = system_time
};

// Process a received packet and return a response packet
Packet process_packet(const Packet *packet) {

    ESP_LOGI(TAG, "Processing packet");
    // Call the corresponding function
    FunctionPointer func = protocol_function_table[packet->function_flag];
    if (func) {
        return func(packet); // Pass the packet to the function
    } else {
        printf("Error: Function not found for flag 0x%02X\n", packet->function_flag);
        return (Packet){0}; // Return an empty packet in case of error
    }
}

Packet heartbeat(const Packet *packet) {
    const char heartbeat_message[] = "HEARTBEAT";

    Packet response = {
        .start_byte = TX_START_BYTE,
        .function_flag = packet->function_flag,
        .payload_size = sizeof(heartbeat_message) - 1
    };

    memset(response.payload, 0, PACKET_SIZE - 4);
    for (size_t i = 0; i < sizeof(heartbeat_message) - 1; ++i) {
        response.payload[i] = heartbeat_message[i];
    }

    return response;
}

Packet system_time(const Packet* packet) {
    static char system_time[64];
    strncpy(system_time, get_timestamp(), sizeof(system_time) - 1);

    ESP_LOGI(TAG, "System time: %s", system_time);

    Packet response = {
        .start_byte = TX_START_BYTE,
        .function_flag = packet->function_flag,
        .payload_size = sizeof(system_time)
    };


    memset(response.payload, 0, PACKET_SIZE - 4);

    memccpy(response.payload, &system_time, sizeof(system_time), sizeof(system_time));

    ESP_LOGI(TAG, "Payload: %s", response.payload);
    ESP_LOGI(TAG, "Returning payload size: %d", response.payload_size);

    return response;
}