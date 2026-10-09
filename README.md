# ☀️ ESP32-C3 Solar Outdoor Weather Sensor Firmware

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Build%20Passing-brightgreen?logo=platformio)](https://platformio.org/)
[![Hardware](https://img.shields.io/badge/Hardware-Seeed%20Studio%20XIAO%20ESP32--C3-blue)](https://www.seeedstudio.com/XIAO-ESP32C3-p-5431.html)
[![Sensor](https://img.shields.io/badge/Sensor-SHT31--DIS%20I2C-orange)](https://www.sensirion.com/products/catalog/SHT31-DIS)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

An ultra-low power, ultra-compact C++ firmware for the **Seeed Studio XIAO ESP32-C3** connected to an **SHT31 Temperature & Humidity Sensor**, powered by a **3.7V Li-Ion battery** and a **5V Solar Panel**.

Designed for 24/7 maintenance-free outdoor operation using ESP32 Deep Sleep, zero-power sensor disconnect, dual-protocol telemetry (**UDP Broadcast** + **HTTPS POST**), and remote temperature calibration.

---

## 🇩🇪 Deutsch

### Hauptmerkmale
* 🔋 **Ultra-Low-Power Deep-Sleep:** Schläft 30 Minuten (konfigurierbar) und schaltet die Stromzufuhr des SHT31-Sensors im Schlaf über Pin `D1` vollständig ab (0 µA Standby).
* 📡 **Dual-Telemetrie:** Sendet die Wetterdaten zeitgleich als blitzschnellen **UDP-Broadcast (Port 8888)** an alle Geräte im Heimnetzwerk UND per **HTTP/HTTPS POST** an einen zentralen Dashboard-Server.
* 📶 **WiFiManager:** Einfaches Einrichten des WLANs über einen automatisch startenden Access-Point (`XIAO-C3-Solar-AP`) ohne erneutes Kompilieren.
* ⚡ **Akku-Spannungsmessung:** 16-faches Oversampling zur Präzisionsmessung der Akkuspannung über einen 1:2 Spannungsteiler an Pin `A2` (mit Prozentumrechnung 3,3V - 4,15V).
* 🌡️ **Taupunkt-Berechnung:** Berechnet den exakten Taupunkt in °C direkt auf dem Mikrocontroller.
* 🛠️ **Wartungsmodus:** Durch Verbinden von Pin `D3` mit Masse (`GND`) bleibt der ESP32 dauerhaft wach für OTA-Updates und serielle Diagnose.
* 🔄 **Remote-Kalibrierung:** Der Temperatur-Offset kann aus der HTTP-Antwort des Servers remote ausgelesen und im EEPROM gespeichert werden.

### Pinbelegung (Seeed Studio XIAO ESP32-C3)
| Pin | Funktion | Beschreibung |
|---|---|---|
| **D1** (GPIO 3) | Sensor VCC | Versorgt den SHT31 nur während der Messung mit Strom |
| **D3** (GPIO 5) | Maintenance Pin | Pull-down an GND hält den ESP32 dauerhaft wach |
| **D4** (GPIO 6) | I2C SDA | Datenseite für SHT31 Sensor (0x44) |
| **D5** (GPIO 7) | I2C SCL | Taktseite für SHT31 Sensor (0x44) |
| **A2** (GPIO 4) | Akku-ADC | Spannungsteiler (2x 200 kΩ) an Li-Ion Akku Plus |
| **5V / VBUS** | Solareingang | 5V Solarpanel über SOD-123 Schottky-Diode |
| **GND** | Masse | Gemeinsame Masse |

### Telemetrie-Datenformat (JSON)
Sowohl per UDP Broadcast (Port `8888`) als auch per HTTP POST wird folgendes JSON-Format übertragen:

```json
{
  "sensor": "solar_outdoor",
  "temperature": 21.5,
  "humidity": 58.2,
  "dewPoint": 12.9,
  "batteryVoltage": 4.12,
  "batteryPercent": 98
}
```

---

## 🇬🇧 English

### Key Features
* 🔋 **Ultra-Low-Power Deep Sleep:** Sleeps for 30 minutes (configurable) and completely cuts SHT31 sensor power via pin `D1` during sleep (0 µA standby).
* 📡 **Dual Telemetry Transmission:** Simultaneously broadcasts weather telemetry via **UDP Broadcast (Port 8888)** to all local network devices AND posts via **HTTP/HTTPS POST** to a central dashboard server.
* 📶 **WiFiManager Integration:** Easy captive portal configuration (`XIAO-C3-Solar-AP`) for credentials and API URLs without re-flashing.
* ⚡ **Precision Battery ADC:** 16-sample oversampling for battery voltage monitoring via 1:2 voltage divider on pin `A2` (0% to 100% mapping for 3.3V - 4.15V Li-Ion cells).
* 🌡️ **Dew Point Calculation:** Calculates exact dew point temperature directly on chip using Magnus-Tetens formula.
* 🛠️ **Maintenance Mode:** Pulling Pin `D3` to `GND` keeps the ESP32 awake indefinitely for debugging, serial monitoring, and OTA updates.
* 🔄 **Remote Offset Calibration:** Server HTTP response can remotely adjust the temperature offset and persist it to EEPROM.

---

## 🛠️ Quick Start & Installation

### 1. Requirements
* [PlatformIO IDE](https://platformio.org/) (VSCode extension or CLI)
* Seeed Studio XIAO ESP32-C3 board
* Sensirion SHT31 Temperature & Humidity Sensor (I2C 0x44)
* 3.7V Li-Ion / LiPo Battery + 5V Solar Panel + SOD-123 Schottky Diode

### 2. Clone & Vorkonfiguration / Configuration
```bash
git clone https://github.com/YOUR-USERNAME/esp32c3-solar-weather-sensor.git
cd esp32c3-solar-weather-sensor
```

Optionally adjust your default server URL or parameters in `include/config.h`:
```cpp
#define DEFAULT_SERVER_URL "http://192.168.178.100:8080/api/tasmota/sensor-push"
#define DEEP_SLEEP_SECONDS 1800
```

### 3. Build & Flash
Connect your Seeed Studio XIAO ESP32-C3 via USB and run:

```bash
# Build firmware
pio run

# Flash to board
pio run --target upload
```

---

## 📜 License
Distributed under the **MIT License**. See `LICENSE` for more information.
