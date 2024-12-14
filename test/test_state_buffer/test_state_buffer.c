#include "unity.h"
#include "defines.h"
#include "StateBuffer.h"

float air_temps[AGG_SAMPLES_PER_METRIC];
bool power_states[AGG_SAMPLES_PER_METRIC];

void setUp() {
    init_state_buffer(time(NULL));
}

void test_insufficient_data() {
    for (int i = 0; i < AGG_SAMPLES_PER_METRIC - 1; i++) {
        add_air_temp((float)i);
        air_temps[i] = (float)i;

        add_power_state(i % 2 == 0);
        power_states[i] = i % 2 == 0;
    }

    // Test that averaging functions return false when not enough data is available
    float average_air_temp;
    time_t dummy_timestamp;
    TEST_ASSERT_FALSE(get_next_minute_average_air_temp(&average_air_temp, &dummy_timestamp));

    float uptime;
    TEST_ASSERT_FALSE(get_next_minute_uptime(&uptime, &dummy_timestamp));
}

void test_averaging() {
    for (int i = 0; i < AGG_SAMPLES_PER_METRIC; i++) {
        add_air_temp((float)i);
        air_temps[i] = (float)i;

        add_power_state(i % 2 == 0);
        power_states[i] = i % 2 == 0;
    }

    float average_air_temp;
    float uptime;
    time_t dummy_timestamp;

    // Test that averaging functions return true when enough data is available
    TEST_ASSERT_TRUE(get_next_minute_average_air_temp(&average_air_temp, &dummy_timestamp));
    TEST_ASSERT_TRUE(get_next_minute_uptime(&uptime, &dummy_timestamp));

    // Test that the average air temperature is correct
    float sum = 0;
    for (int i = 0; i < AGG_SAMPLES_PER_METRIC; i++) {
        sum += air_temps[i];
    }
    TEST_ASSERT_EQUAL_FLOAT(sum / AGG_SAMPLES_PER_METRIC, average_air_temp);

    // Test that the uptime is correct
    int count = 0;
    for (int i = 0; i < AGG_SAMPLES_PER_METRIC; i++) {
        count += power_states[i];
    }
    TEST_ASSERT_EQUAL_FLOAT((float)count / AGG_SAMPLES_PER_METRIC, uptime);
}

void test_time_keeping() {
    time_t start_time = time(NULL);

    init_state_buffer(start_time);

    for (int i = 0; i < AGG_SAMPLES_PER_METRIC * AGG_METRICS_TO_STORE; i++) {
        add_air_temp((float)i);
        add_power_state(i % 2 == 0);
    }

    int seconds_to_overwrite = 5;

    for (int i = 0; i < seconds_to_overwrite; i++) {
        add_air_temp((float)i);
        add_power_state(i % 2 == 0);
    }

    float average_air_temp;
    time_t oldest_air_temp_timestamp;
    
    TEST_ASSERT_TRUE(get_next_minute_average_air_temp(&average_air_temp, &oldest_air_temp_timestamp));
    TEST_ASSERT_EQUAL_INT(start_time + seconds_to_overwrite, oldest_air_temp_timestamp);
    
    float uptime;
    time_t oldest_power_state_timestamp;

    TEST_ASSERT_TRUE(get_next_minute_uptime(&uptime, &oldest_power_state_timestamp));
    TEST_ASSERT_EQUAL_INT(start_time + seconds_to_overwrite, oldest_power_state_timestamp);

}

void app_main() {
    UNITY_BEGIN();
    RUN_TEST(test_insufficient_data);
    RUN_TEST(test_averaging);
    RUN_TEST(test_time_keeping);
    UNITY_END();
}