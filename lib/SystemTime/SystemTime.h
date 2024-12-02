#ifndef SYSTEMTIME_H
#define SYSTEMTIME_H

#include "defines.h"

#include "esp_sntp.h"
#include "esp_log.h"

void time_sync_notification_cb(struct timeval *tv);
void initialize_system_time(void);

const char* get_timestamp(void);

#endif // SYSTEMTIME_H