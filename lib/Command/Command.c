#include "Command.h"

FunctionPointer protocol_function_table[MAX_FUNCTIONS] = {
    [0x00] = heartbeat
};

// Process a received packet and return a response packet
CommandPacket process_packet(const CommandPacket *packet) {

    ESP_LOGI(TAG, "Processing packet");
    // Call the corresponding function
    FunctionPointer func = protocol_function_table[packet->function_flag];
    if (func) {
        return func(packet); // Pass the packet to the function
    } else {
        printf("Error: Function not found for flag 0x%02X\n", packet->function_flag);
        return (CommandPacket){0}; // Return an empty packet in case of error
    }
}

CommandPacket heartbeat(const CommandPacket *packet) {
    const char heartbeat_message[] = "HEARTBEAT";

    CommandPacket response = {
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