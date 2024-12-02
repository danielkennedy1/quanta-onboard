#ifndef WIFI_H
#define WIFI_H

#include "defines.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

typedef struct {
    EventGroupHandle_t wifi_event_group;
    int wifi_connected_bit;
    int retry_count;
} wifi_event_handler_context_t;

void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
void wifi_init_sta(wifi_event_handler_context_t *wifi_event_handler_context);

void initialize_wifi(void);

#endif // WIFI_H