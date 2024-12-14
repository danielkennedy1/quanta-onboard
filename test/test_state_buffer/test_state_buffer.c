#include "unity.h"
#include "defines.h"
#include "StateBuffer.h"

float air_temps[SECONDS_IN_MINUTE];
bool power_states[SECONDS_IN_MINUTE];

void setUp() {
    init_state_buffer();
}

void test_insufficient_data() {
    for (int i = 0; i < SECONDS_IN_MINUTE - 1; i++) {
        add_air_temp((float)i);
        air_temps[i] = (float)i;

        add_power_state(i % 2 == 0);
        power_states[i] = i % 2 == 0;
    }

    // Test that averaging functions return false when not enough data is available
    float average_air_temp;
    TEST_ASSERT_FALSE(get_next_minute_average_air_temp(&average_air_temp));

    float uptime;
    TEST_ASSERT_FALSE(get_next_minute_uptime(&uptime));
}

void test_averaging() {
    for (int i = 0; i < SECONDS_IN_MINUTE; i++) {
        add_air_temp((float)i);
        air_temps[i] = (float)i;

        add_power_state(i % 2 == 0);
        power_states[i] = i % 2 == 0;
    }

    float average_air_temp;
    float uptime;

    // Test that averaging functions return true when enough data is available
    TEST_ASSERT_TRUE(get_next_minute_average_air_temp(&average_air_temp));
    TEST_ASSERT_TRUE(get_next_minute_uptime(&uptime));

    // Test that the average air temperature is correct
    float sum = 0;
    for (int i = 0; i < SECONDS_IN_MINUTE; i++) {
        sum += air_temps[i];
    }
    TEST_ASSERT_EQUAL_FLOAT(sum / SECONDS_IN_MINUTE, average_air_temp);

    // Test that the uptime is correct
    int count = 0;
    for (int i = 0; i < SECONDS_IN_MINUTE; i++) {
        count += power_states[i];
    }
    TEST_ASSERT_EQUAL_FLOAT((float)count / SECONDS_IN_MINUTE, uptime);
}

void app_main() {
    UNITY_BEGIN();
    RUN_TEST(test_insufficient_data);
    RUN_TEST(test_averaging);
    UNITY_END();
}