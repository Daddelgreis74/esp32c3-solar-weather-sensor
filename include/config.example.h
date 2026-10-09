#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Hardware & Pin Definitions (Seeed Studio XIAO ESP32-C3)
// ============================================================================
#define BATTERY_PIN        A2   // Analog Pin for battery voltage divider (GPIO 4 / A2)
#define SENSOR_POWER_PIN   D1   // Power VCC Pin for SHT31 (GPIO 3 - cut power in deep sleep)
#define MAINTENANCE_PIN    D3   // Debug/Maintenance Pin (Pull to GND to keep ESP awake)

// ============================================================================
// Default Network & API Settings
// ============================================================================
// Default Dashboard Server API Endpoint (Can also be updated via WiFiManager / Remote)
#define DEFAULT_SERVER_URL "http://192.168.178.100:8080/api/tasmota/sensor-push"

// UDP Broadcast Port (Broadcasts JSON to local network on port 8888)
#define UDP_BROADCAST_PORT 8888

// WiFiManager Access Point Name (broadcast if WiFi connection fails)
#define AP_NAME            "XIAO-C3-Solar-AP"

// ============================================================================
// Deep Sleep & Power Calibration
// ============================================================================
// Deep Sleep Duration in Seconds (Default: 30 Minutes = 1800 Seconds)
#define DEEP_SLEEP_SECONDS 1800

// Battery Voltage Calibration Multiplier (Resistor Divider 1:2 + Tolerance Correction)
#define BATTERY_CALIBRATION_FACTOR 1.02

// Minimum and Maximum Battery Voltage for 0% - 100% Mapping (in mV)
#define BATTERY_MIN_MV 330
#define BATTERY_MAX_MV 415

#endif // CONFIG_H
