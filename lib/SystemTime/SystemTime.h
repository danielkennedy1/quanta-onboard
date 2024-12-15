#ifndef SYSTEMTIME_H
#define SYSTEMTIME_H

#include "defines.h"

#include "esp_sntp.h"
#include "esp_log.h"

// Call this function to initialize the system time using the SNTP protocol
void initialize_system_time(void);

// Callback function for SNTP synchronization (just logs if event received)
void time_sync_notification_cb(struct timeval *tv);

// Get the current timestamp in the format "YYYY-MM-DD HH:MM:SS"
const char* get_timestamp(void);

#endif // SYSTEMTIME_H