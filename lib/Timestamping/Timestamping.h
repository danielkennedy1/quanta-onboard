#ifndef SNTP_H
#define SNTP_H

#include "esp_sntp.h"
#include "esp_log.h"

void time_sync_notification_cb(struct timeval *tv);
void initialize_system_time(void);

const char* get_timestamp(void);

#endif // SNTP_H