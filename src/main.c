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


#include "Protocol.h"
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
            process_packet(buffer, received);
        }

        // Close client socket
        close(client_sock);
        ESP_LOGI(TAG, "Client disconnected");
    }

    // Cleanup
    close(listen_sock);
    vTaskDelete(NULL);
}

void app_main(void) {

    ESP_ERROR_CHECK(nvs_flash_init());

    wifi_event_handler_context_t wifi_event_handler_context = {
        .wifi_event_group = xEventGroupCreate(),
        .wifi_connected_bit = BIT0,
        .retry_count = 0,
    };

    // Initialize Wi-Fi
    wifi_init_sta(&wifi_event_handler_context);

    // Wait for Wi-Fi connection
    ESP_LOGI(TAG, "CHECKING WIFI CONNECTED BIT");
    xEventGroupWaitBits(wifi_event_handler_context.wifi_event_group, wifi_event_handler_context.wifi_connected_bit, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to Wi-Fi");

    xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
}
