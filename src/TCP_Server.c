#include "TCP_Server.h"

#define TAG "TCP_Server"

static QueueHandles queue_handles;

// TCP server task
void tcp_server_task(void *pvParameters) {

    queue_handles = *(QueueHandles *)pvParameters;

    int listen_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    uint8_t buffer[TCP_BUFFER_SIZE];

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
    server_addr.sin_port = htons(TCP_PORT);
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

    ESP_LOGI(TAG, "TCP server listening on port %d", TCP_PORT);

    while (1) {
        // Accept a new client connection
        // accept() blocks until a client connects to the server
        client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sock < 0) {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        ESP_LOGI(TAG, "New connection accepted");

        // Receive data
        while (1) {
            ssize_t received = recv(client_sock, buffer, TCP_BUFFER_SIZE, 0);
            if (received < 0) {
                ESP_LOGE(TAG, "Error receiving data: errno %d", errno);
                break;
            } else if (received == 0) {
                ESP_LOGI(TAG, "Connection closed");
                break;
            }

            ESP_LOGI(TAG, "Received %d bytes", received);

            Packet* received_packet = from_bytes(buffer, received);

            Command* received_command = malloc(sizeof(Command));
            *received_command = (Command){
                .written = false
            };
            Packet response = process_packet(received_packet, received_command);

            if (received_command->written) {
                // Send command to controller
                ESP_LOGI(TAG, "Sending command to controller");
                if (xQueueSend(queue_handles.command_queue, received_command, 0) != pdTRUE) {
                    ESP_LOGE(TAG, "Error sending command to controller");
                }
            }

            uint8_t* response_bytes = to_bytes(response);

            // Send response
            if (send(client_sock, response_bytes, PACKET_SIZE, 0) < 0) {
                ESP_LOGE(TAG, "Error sending data: errno %d", errno);
                break;
            }

            ESP_LOGI(TAG, "Returned response");

            free(received_packet);
            free(received_command);
            free(response_bytes);
        }

        // Close client socket
        close(client_sock);
        ESP_LOGI(TAG, "Client disconnected");
    }

    // Cleanup
    close(listen_sock);
    vTaskDelete(NULL);
}