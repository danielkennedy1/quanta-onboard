#include "StateBuffer.h"

#define TAG "StateBuffer"

SemaphoreHandle_t air_temp_buffer_mutex;
FloatBuffer air_temp_buffer;

SemaphoreHandle_t power_state_buffer_mutex;
BoolBuffer power_state_buffer;

void init_state_buffer() {
    ESP_LOGI(TAG, "Initializing aggregator");

    air_temp_buffer_mutex = xSemaphoreCreateMutex();
    xSemaphoreTake(air_temp_buffer_mutex, portMAX_DELAY);
    air_temp_buffer.read_head = 0;
    air_temp_buffer.write_head = 0;
    air_temp_buffer.is_empty = true;
    xSemaphoreGive(air_temp_buffer_mutex);

    power_state_buffer_mutex = xSemaphoreCreateMutex();
    xSemaphoreTake(power_state_buffer_mutex, portMAX_DELAY);
    power_state_buffer.read_head = 0;
    power_state_buffer.write_head = 0;
    power_state_buffer.is_empty = true;
    xSemaphoreGive(power_state_buffer_mutex);
}

void add_air_temp(float air_temp) {
    ESP_LOGI(TAG, "Adding air temp: %f", air_temp);
    xSemaphoreTake(air_temp_buffer_mutex, portMAX_DELAY);
    air_temp_buffer.buffer[air_temp_buffer.write_head] = air_temp;
    air_temp_buffer.write_head = (air_temp_buffer.write_head + 1) % (BUFFER_SIZE);
    air_temp_buffer.is_empty = false;

    if (air_temp_buffer.write_head == air_temp_buffer.read_head) {
        air_temp_buffer.read_head = (air_temp_buffer.read_head + 1) % (BUFFER_SIZE);
    }
    xSemaphoreGive(air_temp_buffer_mutex);
}

// Private function
bool read_air_temp(float* air_temp) {
    ESP_LOGI(TAG, "Reading air temp");
    xSemaphoreTake(air_temp_buffer_mutex, portMAX_DELAY);
    if (air_temp_buffer.is_empty) {
        ESP_LOGE(TAG, "Air temp buffer is empty");
        return false;
    }

    *air_temp = air_temp_buffer.buffer[air_temp_buffer.read_head];
    air_temp_buffer.read_head = (air_temp_buffer.read_head + 1) % (BUFFER_SIZE);
    air_temp_buffer.is_empty = (air_temp_buffer.read_head == air_temp_buffer.write_head);

    xSemaphoreGive(air_temp_buffer_mutex);
    return true;
}

void add_power_state(bool power_state) {
    ESP_LOGI(TAG, "Adding power state: %d", power_state);
    xSemaphoreTake(power_state_buffer_mutex, portMAX_DELAY);
    power_state_buffer.buffer[power_state_buffer.write_head] = power_state;
    power_state_buffer.write_head = (power_state_buffer.write_head + 1) % (BUFFER_SIZE);
    power_state_buffer.is_empty = false;

    if (power_state_buffer.write_head == power_state_buffer.read_head) {
        power_state_buffer.read_head = (power_state_buffer.read_head + 1) % (BUFFER_SIZE);
    }
    xSemaphoreGive(power_state_buffer_mutex);
}

// Private function
bool read_power_state(bool* power_state) {
    ESP_LOGI(TAG, "Reading power state");
    xSemaphoreTake(power_state_buffer_mutex, portMAX_DELAY);
    if (power_state_buffer.is_empty) {
        ESP_LOGE(TAG, "Power state buffer is empty");
        return false;
    }

    *power_state = power_state_buffer.buffer[power_state_buffer.read_head];
    power_state_buffer.read_head = (power_state_buffer.read_head + 1) % (BUFFER_SIZE);
    power_state_buffer.is_empty = (power_state_buffer.read_head == power_state_buffer.write_head);

    xSemaphoreGive(power_state_buffer_mutex);
    return true;
}

bool get_next_minute_average_air_temp(float* average_air_temp) {
    if (air_temp_buffer.is_empty) {
        ESP_LOGE(TAG, "Air temp buffer is empty");
        return false;
    }

    bool minute_of_data_available = false;

    // read head is behind write head
    if (air_temp_buffer.read_head <= air_temp_buffer.write_head) {
        minute_of_data_available = (air_temp_buffer.write_head - air_temp_buffer.read_head) >= SECONDS_IN_MINUTE;
    }

    // read head is ahead of write head
    if (air_temp_buffer.read_head > air_temp_buffer.write_head) {
        minute_of_data_available = (BUFFER_SIZE - air_temp_buffer.read_head + air_temp_buffer.write_head) >= SECONDS_IN_MINUTE;
    }

    if (!minute_of_data_available) {
        ESP_LOGE(TAG, "Minute of data not available");
        return false;
    }

    float sum = 0;
    
    for (int i = 0; i < SECONDS_IN_MINUTE; i++) {
        sum += air_temp_buffer.buffer[(air_temp_buffer.read_head + i) % BUFFER_SIZE];
    }

    *average_air_temp = sum / SECONDS_IN_MINUTE;

    return true;
}

bool get_next_minute_uptime(float* uptime) {
    if (power_state_buffer.is_empty) {
        ESP_LOGE(TAG, "Power state buffer is empty");
        return false;
    }

    bool minute_of_data_available = false;

    // read head is behind write head
    if (power_state_buffer.read_head <= power_state_buffer.write_head) {
        minute_of_data_available = (power_state_buffer.write_head - power_state_buffer.read_head) >= SECONDS_IN_MINUTE;
    }

    // read head is ahead of write head
    if (power_state_buffer.read_head > power_state_buffer.write_head) {
        minute_of_data_available = (BUFFER_SIZE - power_state_buffer.read_head + power_state_buffer.write_head) >= SECONDS_IN_MINUTE;
    }

    if (!minute_of_data_available) {
        ESP_LOGE(TAG, "Minute of data not available");
        return false;
    }

    int uptime_count = 0;

    for (int i = 0; i < SECONDS_IN_MINUTE; i++) {
        uptime_count += power_state_buffer.buffer[(power_state_buffer.read_head + i) % BUFFER_SIZE];
    }

    *uptime = (float)uptime_count / SECONDS_IN_MINUTE;

    return true;
}