#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "defines.h"

#include <string.h>

#include "esp_log.h"
#include "lwip/sockets.h"

#include "Protocol.h"
#include "Command.h"

void tcp_server_task(void *pvParameters);

#endif // TCP_SERVER_H