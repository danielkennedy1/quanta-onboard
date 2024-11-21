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

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    wifi_event_handler_context_t *wifi_event_handler_context = (wifi_event_handler_context_t *)arg;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (wifi_event_handler_context->retry_count < MAX_RETRY) {
            esp_wifi_connect();
            wifi_event_handler_context->retry_count++;
            ESP_LOGI(TAG, "Retrying connection to Wi-Fi...");
        } else {
            ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        wifi_event_handler_context->retry_count = 0;
        ESP_LOGI(TAG, "SETTING WIFI CONNECTED BIT");
        xEventGroupSetBits(wifi_event_handler_context->wifi_event_group, wifi_event_handler_context->wifi_connected_bit);
    }
}

// Initialize Wi-Fi
void wifi_init_sta(wifi_event_handler_context_t *wifi_event_handler_context) {

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        wifi_event_handler_context,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        wifi_event_handler_context,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi initialization complete.");
}

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
    ESP_LOGI(TAG, "%d", fake_function());

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
