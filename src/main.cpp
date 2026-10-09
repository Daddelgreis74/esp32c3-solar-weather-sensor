#include <WiFi.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <WiFiManager.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>
#include <EEPROM.h>
#include <math.h>

#include "config.h"

// Global instances & configuration variables
Adafruit_SHT31 sht31 = Adafruit_SHT31();
WiFiUDP udp;

float tempOffset = 0.0;
String serverUrl = "";

// Calculate Dew Point temperature in Celsius
float calculateDewPoint(float temp, float hum) {
    float a = 17.625;
    float b = 243.04;
    float alpha = ((a * temp) / (b + temp)) + log(hum / 100.0);
    return (b * alpha) / (a - alpha);
}

// Write string to EEPROM with length byte header
void writeStringEEPROM(int addr, String str) {
    byte len = str.length();
    EEPROM.write(addr, len);
    for (int i = 0; i < len; i++) {
        EEPROM.write(addr + 1 + i, str[i]);
    }
    EEPROM.write(addr + 1 + len, 0);
}

// Read string from EEPROM with length verification
String readStringEEPROM(int addr) {
    byte len = EEPROM.read(addr);
    if (len == 255 || len == 0 || len > 120) return "";
    String str = "";
    for (int i = 0; i < len; i++) {
        str += (char)EEPROM.read(addr + 1 + i);
    }
    return str;
}

// Measure battery voltage using 16-sample averaging
float getBatteryVoltage() {
    uint32_t Vbatt = 0;
    for (int i = 0; i < 16; i++) {
        Vbatt += analogReadMilliVolts(BATTERY_PIN);
        delay(5);
    }
    // 2.0x factor for 1/2 resistor voltage divider + calibration factor
    float voltage = 2.0 * (Vbatt / 16.0) / 1000.0 * BATTERY_CALIBRATION_FACTOR;
    return voltage;
}

// Convert battery voltage to 0% - 100% percentage
int getBatteryPercent(float voltage) {
    int pct = map(voltage * 100, BATTERY_MIN_MV, BATTERY_MAX_MV, 0, 100);
    return constrain(pct, 0, 100);
}

// Send weather telemetry via UDP Broadcast and HTTP/HTTPS POST
void sendData(float temp, float hum, float dp, float vBat, int pBat) {
    Serial.println("Preparing telemetry payload...");
    
    String tempStr = isnan(temp) ? "null" : String(temp, 1);
    String humStr = isnan(hum) ? "null" : String(hum, 1);
    String dpStr = isnan(dp) ? "null" : String(dp, 1);

    String payload = "{\"sensor\":\"solar_outdoor\""
                     ",\"temperature\":" + tempStr + 
                     ",\"humidity\":" + humStr + 
                     ",\"dewPoint\":" + dpStr + 
                     ",\"batteryVoltage\":" + String(vBat, 2) + 
                     ",\"batteryPercent\":" + String(pBat) + "}";

    // 1. Send UDP Broadcast to all devices on local network
    if (WiFi.status() == WL_CONNECTED) {
        IPAddress broadcastIP = ~WiFi.subnetMask() | WiFi.localIP();
        udp.beginPacket(broadcastIP, UDP_BROADCAST_PORT);
        udp.write((const uint8_t*)payload.c_str(), payload.length());
        udp.endPacket();
        Serial.printf("UDP-Broadcast sent to %s:%d\n", broadcastIP.toString().c_str(), UDP_BROADCAST_PORT);
    }

    // 2. Send HTTP/HTTPS POST to configured Dashboard Server
    int httpResponseCode = -1;
    String response = "";

    Serial.println("Sending HTTP POST to: " + serverUrl);
    
    if (serverUrl.startsWith("https://")) {
        WiFiClientSecure client;
        client.setInsecure(); // Bypass SSL verification for internal IP self-signed certificates
        HTTPClient http;
        if (http.begin(client, serverUrl)) {
            http.addHeader("Content-Type", "application/json");
            httpResponseCode = http.POST(payload);
            response = http.getString();
            http.end();
        }
    } else {
        WiFiClient client;
        HTTPClient http;
        if (http.begin(client, serverUrl)) {
            http.addHeader("Content-Type", "application/json");
            httpResponseCode = http.POST(payload);
            response = http.getString();
            http.end();
        }
    }
    
    if (httpResponseCode > 0) {
        Serial.printf("Server Response (%d): %s\n", httpResponseCode, response.c_str());
        
        // Process potential remote tempOffset adjustment from server response
        int offsetIdx = response.indexOf("\"tempOffset\":");
        if (offsetIdx != -1) {
            int startIdx = offsetIdx + 13;
            int endIdx = response.indexOf("}", startIdx);
            if (endIdx == -1) endIdx = response.indexOf(",", startIdx);
            if (endIdx != -1) {
                String valStr = response.substring(startIdx, endIdx);
                float newOffset = valStr.toFloat();
                if (newOffset != tempOffset && newOffset >= -50.0 && newOffset <= 50.0) {
                    tempOffset = newOffset;
                    EEPROM.put(0, tempOffset);
                    EEPROM.commit();
                    Serial.printf("Temperature offset remotely updated to: %.1f C\n", tempOffset);
                }
            }
        }
    } else {
        Serial.printf("HTTP Post Error: %s\n", HTTPClient::errorToString(httpResponseCode).c_str());
    }
}

void setup() {
    Serial.begin(115200);
    delay(2000); 
    
    Serial.println("\n\n=== Seeed Studio XIAO ESP32-C3 Solar Weather Sensor ===");
    
    // Power up SHT31 sensor via D1 pin
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    digitalWrite(SENSOR_POWER_PIN, HIGH);
    delay(50);
    
    EEPROM.begin(512);
    EEPROM.get(0, tempOffset);
    if (isnan(tempOffset) || tempOffset < -50.0 || tempOffset > 50.0) {
        tempOffset = 0.0;
        EEPROM.put(0, tempOffset);
        EEPROM.commit();
    }
    
    serverUrl = readStringEEPROM(10);
    if (serverUrl == "" || !serverUrl.startsWith("http")) {
        serverUrl = DEFAULT_SERVER_URL;
        writeStringEEPROM(10, serverUrl);
        EEPROM.commit();
    }
    
    // Initialize I2C and SHT31 sensor
    Wire.begin();
    bool sensorFound = sht31.begin(0x44);
    if (!sensorFound) {
        Serial.println("WARNING: SHT31 sensor not detected at I2C address 0x44.");
    } else {
        Serial.println("SHT31 sensor initialized successfully.");
    }
    
    pinMode(BATTERY_PIN, INPUT);
    
    // Check maintenance mode pin D3 (Pull to GND to keep awake)
    pinMode(MAINTENANCE_PIN, INPUT_PULLUP);
    bool debugMode = (digitalRead(MAINTENANCE_PIN) == LOW);
    
    WiFiManager wm;
    char offsetStr[10];
    dtostrf(tempOffset, 1, 1, offsetStr);
    WiFiManagerParameter custom_server_url("server", "Dashboard API URL", serverUrl.c_str(), 120);
    WiFiManagerParameter custom_temp_offset("offset", "Temperature Offset (C)", offsetStr, 10);
    
    wm.addParameter(&custom_server_url);
    wm.addParameter(&custom_temp_offset);
    
    // 3 minute portal timeout to preserve battery if WiFi is unavailable
    wm.setConfigPortalTimeout(180);
    
    Serial.println("Connecting to WiFi...");
    if (!wm.autoConnect(AP_NAME)) {
        Serial.println("WiFi connection failed. Entering deep sleep...");
        digitalWrite(SENSOR_POWER_PIN, LOW);
        if (!debugMode) {
            esp_sleep_enable_timer_wakeup((uint64_t)DEEP_SLEEP_SECONDS * 1000000ULL);
            esp_deep_sleep_start();
        }
    }
    Serial.println("WiFi connected successfully!");
    Serial.print("Local IP Address: ");
    Serial.println(WiFi.localIP());
    
    String newUrl = String(custom_server_url.getValue());
    newUrl.trim();
    float newOffset = String(custom_temp_offset.getValue()).toFloat();
    
    bool needsCommit = false;
    if (newUrl != serverUrl && newUrl.length() > 0) {
        serverUrl = newUrl;
        writeStringEEPROM(10, serverUrl);
        needsCommit = true;
    }
    if (newOffset != tempOffset && newOffset >= -50.0 && newOffset <= 50.0) {
        tempOffset = newOffset;
        EEPROM.put(0, tempOffset);
        needsCommit = true;
    }
    if (needsCommit) {
        EEPROM.commit();
        Serial.println("Updated configuration saved to EEPROM.");
    }
    
    // Read sensor metrics
    float rawTemp = NAN;
    float hum = NAN;
    
    if (sensorFound) {
        rawTemp = sht31.readTemperature();
        hum = sht31.readHumidity();
    }
    
    float vBat = getBatteryVoltage();
    int pBat = getBatteryPercent(vBat);
    
    float currentTemp = NAN;
    float dp = NAN;
    if (!isnan(rawTemp)) {
        currentTemp = rawTemp + tempOffset;
    }
    if (!isnan(currentTemp) && !isnan(hum)) {
        dp = calculateDewPoint(currentTemp, hum);
    }
    
    if (isnan(rawTemp) || isnan(hum)) {
        Serial.println("Failed to read SHT31 sensor! Sending battery metrics only.");
        Serial.printf("Battery - Voltage: %.2f V | Charge: %d %%\n", vBat, pBat);
    } else {
        Serial.printf("Telemetry - Temp: %.1f C | Humidity: %.1f %% | DewPoint: %.1f C\n", currentTemp, hum, dp);
        Serial.printf("Battery   - Voltage: %.2f V | Charge: %d %%\n", vBat, pBat);
    }
    
    sendData(currentTemp, hum, dp, vBat, pBat);
    
    // Power down sensor before entering deep sleep
    digitalWrite(SENSOR_POWER_PIN, LOW);
    Serial.flush();
    
    if (debugMode) {
        Serial.println("Maintenance Pin D3 pulled LOW. Debug mode active - staying awake.");
        digitalWrite(SENSOR_POWER_PIN, HIGH);
    } else {
        Serial.printf("Entering deep sleep for %d seconds...\n", DEEP_SLEEP_SECONDS);
        WiFi.disconnect(true);
        delay(100);
        
        esp_sleep_enable_timer_wakeup((uint64_t)DEEP_SLEEP_SECONDS * 1000000ULL);
        esp_deep_sleep_start();
    }
}

void loop() {
    // Maintenance loop (active only when D3 is pulled to GND)
    delay(1000);
    Serial.println("Maintenance mode active... loop() running.");
}
