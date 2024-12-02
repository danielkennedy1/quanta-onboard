#include "defines.h"

#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "freertos/task.h"
#include "freertos/queue.h"
#include "lwip/sockets.h"


#include "Command.h"
#include "Wifi.h"


// TCP server task
void tcp_server_task(void *pvParameters) {
    int listen_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    uint8_t buffer[BUFFER_SIZE];

    // Create socket
    listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (listen_sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }

    // Bind socket to port
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
        return;
    }

    // Listen for incoming connections
    if (listen(listen_sock, 1) < 0) {
        ESP_LOGE(TAG, "Error in listen: errno %d", errno);
        close(listen_sock);
        vTaskDelete(NULL);
        return;
    }

    ESP_LOGI(TAG, "TCP server listening on port %d", PORT);

    while (1) {
        // Accept a new client connection
        client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        ESP_LOGI(TAG, "New connection accepted");

        // Receive data
        while (1) {
            ssize_t received = recv(client_sock, buffer, BUFFER_SIZE, 0);
            if (received < 0) {
                ESP_LOGE(TAG, "Error receiving data: errno %d", errno);
                break;
            } else if (received == 0) {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }

            ESP_LOGI(TAG, "Received %d bytes", received);

            // Process received data
            //process_packet(buffer, received);
        }

        // Close client socket
        close(client_sock);
        ESP_LOGI(TAG, "Client disconnected");
    }

    // Cleanup
    close(listen_sock);
    vTaskDelete(NULL);
}

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

    CommandPacket sent_packet = {
        .start_byte = RX_START_BYTE,
        .function_flag = 0x00,
        .payload_size = 0
    };
    //memset(sent_packet.payload, 0, PACKET_SIZE - 4);
    //int payload[12] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c};

    // for (size_t i = 0; i < sent_packet.payload_size; i++) {
    //     sent_packet.payload[i] = payload[i];
    // }
    // ESP_LOGI(TAG, "Serializing packet");
    
    uint8_t *bytes = to_bytes(sent_packet);

    ESP_LOGI(TAG, "Bytes: ");
    for (size_t i = 0; i < PACKET_SIZE; i++) {
        printf("%02x ", bytes[i]);
    }
    printf("\n");

    ESP_LOGI(TAG, "Deserializing packet");

    CommandPacket* received_packet = from_bytes(bytes, PACKET_SIZE);

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

    CommandPacket response = process_packet(received_packet);

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
