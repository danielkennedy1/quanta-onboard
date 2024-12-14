#ifndef DEFINES_H
#define DEFINES_H

/*
 * Replace the following constants with your own values,
 * then rename this file to defines.h
 */

// Connectivity configuration
#define WIFI_SSID "The SSID of your Wi-Fi network"
#define WIFI_PASS "The password of your Wi-Fi network"
#define WIFI_MAX_RETRY 20 // Maximum number of Wi-Fi connection retries
#define SNTP_SERVER "pool.ntp.org"  // Default NTP server
#define SNTP_MAX_RETRY 10 // Number of SNTP synchronization retries
#define PORT 1234 // TCP server port
#define BUFFER_SIZE 128 // TCP server buffer size


// Initial heater state configuration
#define STATE_INITIAL_ON false // Initial heater set control state (true/false)
#define STATE_INITIAL_TARGET_TEMP 25.0 // Initial target temperature (Celsius)
#define STATE_INITIAL_CONTROL_MODE SET_CONTROL // Initial control mode (SET_CONTROL/SET_TARGET_TEMP)

// Heater configuration
#define HEATER_WATTAGE 10 // Heater wattage (W)
#define HEATER_THERMOSTAT_INTERVAL 1 // Heater thermostat interval (s)

// Air temperature simulation configuration
#define SIM_AIR_INITIAL_TEMP 25.0 // Initial air temperature (Celsius)
#define SIM_AIR_HEAT_CAPACITY 1234.56 // Heat capacity of the air (J/C)
#define SIM_AIR_HEAT_TRANSFER 1 // Heat loss transfer rate (W)
#define SIM_AIR_TEMP_MIN 0.0 // Minimum air temperature (Celsius)
#define SIM_AIR_TEMP_MAX 100.0 // Maximum air temperature (Celsius)
#define SIM_TIME_INTERVAL 1 // Time interval (s)

// Data aggregation configuration
#define AGG_SAMPLES_PER_SECOND 1 // Number of samples per second (1 second)
#define AGG_SAMPLES_PER_METRIC 60 // Number of samples per metric (1 minute)
#define AGG_METRICS_TO_STORE 10   // Number of metrics to store (10 minutes)

#endif // DEFINES_H