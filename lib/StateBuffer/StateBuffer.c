#include "StateBuffer.h"

#define TAG "StateBuffer"

SemaphoreHandle_t air_temp_buffer_mutex;
FloatBuffer air_temp_buffer;
time_t oldest_air_temp_time;

SemaphoreHandle_t power_state_buffer_mutex;
BoolBuffer power_state_buffer;
time_t oldest_power_state_time;

void init_state_buffer(time_t current_time) {
    ESP_LOGI(TAG, "Initializing aggregator");

    air_temp_buffer_mutex = xSemaphoreCreateMutex();
    xSemaphoreTake(air_temp_buffer_mutex, portMAX_DELAY);
    air_temp_buffer.read_head = 0;
    air_temp_buffer.write_head = 0;
    air_temp_buffer.is_empty = true;
    oldest_air_temp_time = current_time;
    xSemaphoreGive(air_temp_buffer_mutex);

    power_state_buffer_mutex = xSemaphoreCreateMutex();
    xSemaphoreTake(power_state_buffer_mutex, portMAX_DELAY);
    power_state_buffer.read_head = 0;
    power_state_buffer.write_head = 0;
    power_state_buffer.is_empty = true;
    oldest_power_state_time = current_time;
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
        oldest_air_temp_time++;
    }
    xSemaphoreGive(air_temp_buffer_mutex);
}

// Private function
bool read_air_temp(float* air_temp) {
    ESP_LOGD(TAG, "Reading air temp");
    xSemaphoreTake(air_temp_buffer_mutex, portMAX_DELAY);
    if (air_temp_buffer.is_empty) {
        ESP_LOGE(TAG, "Air temp buffer is empty");
        return false;
    }

    *air_temp = air_temp_buffer.buffer[air_temp_buffer.read_head];
    air_temp_buffer.read_head = (air_temp_buffer.read_head + 1) % (BUFFER_SIZE);
    air_temp_buffer.is_empty = (air_temp_buffer.read_head == air_temp_buffer.write_head);
    oldest_air_temp_time++;

    xSemaphoreGive(air_temp_buffer_mutex);
    return true;
}

void add_power_state(bool power_state) {
    ESP_LOGD(TAG, "Adding power state: %d", power_state);
    xSemaphoreTake(power_state_buffer_mutex, portMAX_DELAY);
    power_state_buffer.buffer[power_state_buffer.write_head] = power_state;
    power_state_buffer.write_head = (power_state_buffer.write_head + 1) % (BUFFER_SIZE);
    power_state_buffer.is_empty = false;

    if (power_state_buffer.write_head == power_state_buffer.read_head) {
        power_state_buffer.read_head = (power_state_buffer.read_head + 1) % (BUFFER_SIZE);
        oldest_power_state_time++;
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
    oldest_power_state_time++;

    xSemaphoreGive(power_state_buffer_mutex);
    return true;
}

bool get_next_minute_average_air_temp(float* average_air_temp, time_t* timestamp) {
    if (air_temp_buffer.is_empty) {
        ESP_LOGE(TAG, "Air temp buffer is empty");
        return false;
    }

    bool minute_of_data_available = false;

    // read head is behind write head
    if (air_temp_buffer.read_head <= air_temp_buffer.write_head) {
        minute_of_data_available = (air_temp_buffer.write_head - air_temp_buffer.read_head) >= AGG_SAMPLES_PER_METRIC;
    }

    // read head is ahead of write head
    if (air_temp_buffer.read_head > air_temp_buffer.write_head) {
        minute_of_data_available = (BUFFER_SIZE - air_temp_buffer.read_head + air_temp_buffer.write_head) >= AGG_SAMPLES_PER_METRIC;
    }

    if (!minute_of_data_available) {
        ESP_LOGE(TAG, "Minute of data not available");
        return false;
    }

    float sum = 0;
    *timestamp = oldest_air_temp_time - 1; // Subtract 1 to get the timestamp of the oldest data point extracted

    
    for (int i = 0; i < AGG_SAMPLES_PER_METRIC; i++) {
        float air_temp;
        read_air_temp(&air_temp);
        sum += air_temp;
    }

    *average_air_temp = sum / AGG_SAMPLES_PER_METRIC;

    return true;
}

bool get_next_minute_uptime(float* uptime, time_t* timestamp) {
    if (power_state_buffer.is_empty) {
        ESP_LOGE(TAG, "Power state buffer is empty");
        return false;
    }

    bool minute_of_data_available = false;

    // read head is behind write head
    if (power_state_buffer.read_head <= power_state_buffer.write_head) {
        minute_of_data_available = (power_state_buffer.write_head - power_state_buffer.read_head) >= AGG_SAMPLES_PER_METRIC;
    }

    // read head is ahead of write head
    if (power_state_buffer.read_head > power_state_buffer.write_head) {
        minute_of_data_available = (BUFFER_SIZE - power_state_buffer.read_head + power_state_buffer.write_head) >= AGG_SAMPLES_PER_METRIC;
    }

    if (!minute_of_data_available) {
        ESP_LOGE(TAG, "Minute of data not available");
        return false;
    }

    int uptime_count = 0;
    *timestamp = oldest_power_state_time - 1; // Subtract 1 to get the timestamp of the oldest data point extracted


    for (int i = 0; i < AGG_SAMPLES_PER_METRIC; i++) {
        bool power_state;
        read_power_state(&power_state);
        uptime_count += power_state;
    }

    *uptime = (float)uptime_count / AGG_SAMPLES_PER_METRIC;

    return true;
}