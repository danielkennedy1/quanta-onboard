#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "defines.h"

#include <string.h>

#include "esp_log.h"
#include "lwip/sockets.h"

#include "Protocol.h"
#include "Command.h"

/*
    TCP server task
    This task will listen for incoming connections and receive data
    The data will be processed and a response will be sent back
    If the process_task call writes the command, the command will be queued for the controller task    
*/
void tcp_server_task(void *pvParameters);

#endif // TCP_SERVER_H