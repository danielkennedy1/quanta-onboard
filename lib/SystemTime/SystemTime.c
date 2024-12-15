#include "SystemTime.h"

#define TAG "TIME"

void time_sync_notification_cb(struct timeval *tv) {
    ESP_LOGI(TAG, "Time synchronization event received");
}

void initialize_system_time(void) {
    ESP_LOGI(TAG, "Initializing System time using SNTP");

    esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, SNTP_SERVER);
    esp_sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    esp_sntp_init();

    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = SNTP_MAX_RETRY;

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Wait 2 seconds
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (timeinfo.tm_year < (2016 - 1900)) {
        ESP_LOGE(TAG, "Failed to synchronize time");
    } else {
        ESP_LOGI(TAG, "Time synchronized successfully");
    }
}

const char* get_timestamp(void) {
    time_t now;
    struct tm timestamp;
    static char timestamp_str[64]; // Static buffer to store timestamp string, no reallocation for each call

    time(&now);
    localtime_r(&now, &timestamp);

    strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%dT%H:%M:%S.000%z", &timestamp);
    ESP_LOGI(TAG, "The current date/time is: %s", timestamp_str);

    return timestamp_str;
}