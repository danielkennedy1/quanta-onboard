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

#include "defines.h"

#include "Protocol.h"

static const char *TAG = "WIFI_TCP";
static EventGroupHandle_t wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static int retry_count = 0;

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (retry_count < MAX_RETRY) {
            esp_wifi_connect();
            retry_count++;
            ESP_LOGI(TAG, "Retrying connection to Wi-Fi...");
        } else {
            ESP_LOGE(TAG, "Failed to connect to Wi-Fi");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        retry_count = 0;
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Initialize Wi-Fi
void wifi_init_sta(void) {
    wifi_event_group = xEventGroupCreate();

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
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
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


// Custom Protocol Constants
#define START_BYTE 0x02



// Function to process received packet
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

    // Initialize Wi-Fi
    wifi_init_sta();

    // Wait for Wi-Fi connection
    xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT,
                        false, true, portMAX_DELAY);
    ESP_LOGI(TAG, "Connected to Wi-Fi");

    xTaskCreate(tcp_server_task, "tcp_server_task", 4096, NULL, 5, NULL);
}
