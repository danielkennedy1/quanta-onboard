#include "unity.h"
#include "defines.h"
#include "Protocol.h"
#include "Command.h"

void test_heartbeat() {
    Packet start_packet = {
        .start_byte = RX_START_BYTE,
        .function_flag = 0x00,
        .payload_size = 0
    };

    uint8_t *bytes = to_bytes(start_packet);

    Packet* rx_packet = from_bytes(bytes, PACKET_SIZE);

    Command* command = malloc(sizeof(Command));

    Packet processed_packet = process_packet(rx_packet, command);

    TEST_ASSERT_EQUAL_HEX8(TX_START_BYTE, processed_packet.start_byte);
    TEST_ASSERT_EQUAL_HEX8(0x00, processed_packet.function_flag);
    TEST_ASSERT_EQUAL(9, processed_packet.payload_size);

    TEST_ASSERT_EQUAL_STRING_LEN("HEARTBEAT", processed_packet.payload, processed_packet.payload_size);

    free(bytes);
}

void app_main() {
    UNITY_BEGIN();
    RUN_TEST(test_heartbeat);
    UNITY_END();
}