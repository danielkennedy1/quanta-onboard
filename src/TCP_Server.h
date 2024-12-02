#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <string.h>

// ESP32 libraries
#include "esp_log.h"
#include "lwip/sockets.h"

#include "defines.h"

void tcp_server_task(void *pvParameters);

#endif // TCP_SERVER_H