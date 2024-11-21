#ifndef WIFI_H
#define WIFI_H

typedef struct {
    EventGroupHandle_t wifi_event_group;
    int wifi_connected_bit;
    int retry_count;
} wifi_event_handler_context_t;

#endif // WIFI_H