#include "defines.h"

// C standard libraries
#include <stdio.h>
#include <string.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

// ESP32 libraries
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Tasks
#include "TCP_Server.h"

// Libraries
#include "Command.h"
#include "Wifi.h"

#define TAG "main"

#define WIFI false
#define SERVER false

void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());

    if (WIFI) {
        // Initialize Wi-Fi
        wifi_event_handler_context_t wifi_event_handler_context = {
            .wifi_event_group = xEventGroupCreate(),
            .wifi_connected_bit = BIT0,
            .retry_count = 0,
        };
        wifi_init_sta(&wifi_event_handler_context);

        // Wait for Wi-Fi connection
        ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");
        xEventGroupWaitBits(wifi_event_handler_context.wifi_event_group, wifi_event_handler_context.wifi_connected_bit, pdFALSE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Connected to Wi-Fi");
    }

    if (SERVER) {
        xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
    }

    ESP_LOGI(TAG, "Making a command packet");

    Packet sent_packet = {
        .start_byte = RX_START_BYTE,
        .function_flag = 0x00,
        .payload_size = 0
    };
    
    uint8_t *bytes = to_bytes(sent_packet);

    ESP_LOGI(TAG, "Bytes: ");
    for (size_t i = 0; i < PACKET_SIZE; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");

    ESP_LOGI(TAG, "Deserializing packet");

    Packet* received_packet = from_bytes(bytes, PACKET_SIZE);

    ESP_LOGI(TAG, "Received packet:");
    ESP_LOGI(TAG, "Function flag: %02x", received_packet->function_flag);
    ESP_LOGI(TAG, "Payload size: %02x", received_packet->payload_size);

    ESP_LOGI(TAG, "Payload:");
    for (size_t i = 0; i < received_packet->payload_size; i++) {
        printf("%02x ", received_packet->payload[i]);
    }
    printf("\n");

    free(bytes);

    ESP_LOGI(TAG, "Processing packet");

    Packet response = process_packet(received_packet);

    ESP_LOGI(TAG, "Response:");
    ESP_LOGI(TAG, "Function flag: %02x", response.function_flag);
    ESP_LOGI(TAG, "Payload size: %02x", response.payload_size);

    ESP_LOGI(TAG, "Payload:");
    for (size_t i = 0; i < response.payload_size; i++) {
        printf("%02x ", response.payload[i]);
    }

    free(received_packet);

    ESP_LOGI(TAG, "Done");
}
