# ☀️ ESP32-C3 Solar Outdoor Weather Sensor Firmware

[![PlatformIO](https://img.shields.io/badge/PlatformIO-Build%20Passing-brightgreen?logo=platformio)](https://platformio.org/)
[![Hardware](https://img.shields.io/badge/Hardware-Seeed%20Studio%20XIAO%20ESP32--C3-blue)](https://www.seeedstudio.com/XIAO-ESP32C3-p-5431.html)
[![Sensor](https://img.shields.io/badge/Sensor-SHT31--DIS%20I2C-orange)](https://www.sensirion.com/products/catalog/SHT31-DIS)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

An ultra-low power, ultra-compact C++ firmware for the **Seeed Studio XIAO ESP32-C3** connected to an **SHT31 Temperature & Humidity Sensor**, powered by a **3.7V Li-Ion battery** and a **5V Solar Panel**.

Designed for 24/7 maintenance-free outdoor operation using ESP32 Deep Sleep, zero-power sensor disconnect, dual-protocol telemetry (**UDP Broadcast** + **HTTPS POST**), and remote temperature calibration.

---

## 🇩🇪 Deutsch

### 🛒 Benötigte Hardware-Komponenten (Stückliste / BOM)

| Komponente | Bezeichnung / Spezialisierung | Bauform / Spezifikation | Funktion im Projekt |
|---|---|---|---|
| **Mikrocontroller** | **Seeed Studio XIAO ESP32-C3** | USB-C, RISC-V 160MHz, 4MB Flash, 2.54mm Pitch | Hauptprozessor mit WLAN, Deep-Sleep & Akkuladeelektronik |
| **Sensor** | **Sensirion SHT31-DIS** Modul | I2C (Adresse `0x44`), 4-Pin Anschluss | Präzisions-Messung von Temperatur (-40 bis +125 °C) & Luftfeuchtigkeit (0-100%) |
| **Solarpanel** | **5V Solarpanel** | 5V / 1W bis 3W (z. B. 110x60 mm) | Lädt den Akku tagsüber bei Sonneneinstrahlung nach |
| **Akku** | **3,7V Li-Ion / LiPo Akku** | 3.7V nominal (z. B. 18650 Zelle 2200-3000 mAh oder LiPo-Pack) | Unterbrechungsfreie Stromversorgung für Tag & Nacht |
| **Schutzdiode** | **Schottky-Diode** | SOD-123 (z. B. `1N5819W` oder `SS14`) | Verhindert Rückstrom vom Akku in das Solarpanel bei Nacht |
| **Widerstände (2x)** | **200 kΩ SMD-Widerstände** | 0805 SMD (2 Stück) | 1:2 Spannungsteiler an Pin `A2` für präzise Akkuspannungsmessung |
| **Schalter / Jumper** | **Wartungsschalter / Jumper** | 2-Pin Stiftleiste (2.54mm) + Jumper-Brücke | Schaltet Pin `D3` auf `GND` für Dauerbetrieb / OTA-Updates / Serial Monitor |
| **Akku-Stecker** | **JST-PH 2.0mm** Steckverbinder | 2-Pin Gewinkelt / Vertikal | Sichere Steckverbindung für den Li-Ion Akku |
| **Solar-Klemme** | **2-Pin Schraubklemme** | 5.08mm Pitch (z. B. Phoenix Contact) | Einfacher Anschluss für die Kabel des Solarpanels |
| **Buchsenleisten** | **2x 7-Pin Buchsenleisten** | 2.54mm Pitch | Gesockelte Montage des XIAO ESP32-C3 Moduls |

---

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

### 🛒 Hardware Components (Bill of Materials / BOM)

| Component | Part / Model | Package / Form Factor | Function |
|---|---|---|---|
| **Microcontroller** | **Seeed Studio XIAO ESP32-C3** | USB-C, RISC-V 160MHz, 4MB Flash, 2.54mm Pitch | Core MCU with WiFi, BLE, Deep-Sleep & Onboard Li-Ion Charger |
| **Sensor** | **Sensirion SHT31-DIS** Module | I2C (Address `0x44`), 4-Pin Header | Precision Temperature (-40 to +125 °C) & Humidity (0-100%) sensor |
| **Solar Panel** | **5V Solar Panel** | 5V / 1W to 3W (e.g., 110x60 mm) | Charges the 3.7V battery during daylight |
| **Battery** | **3.7V Li-Ion / LiPo Battery** | 3.7V Nominal (18650 cell 2200-3000 mAh or LiPo pack) | Uninterruptible power supply for day & night operation |
| **Protection Diode** | **Schottky Barrier Diode** | SOD-123 (`1N5819W` or `SS14`) | Prevents reverse battery current leakage into solar panel at night |
| **Resistors (2x)** | **200 kΩ SMD Resistors** | 0805 SMD (2 pieces) | 1:2 Voltage divider on Pin `A2` for safe 0-3.3V ADC battery sensing |
| **Switch / Jumper** | **Maintenance Switch / Jumper** | 2-Pin 2.54mm Pin Header + Jumper Shunt | Pulls Pin `D3` to `GND` to keep MCU awake for OTA / Serial Monitor |
| **Battery Header** | **JST-PH 2.0mm** Connector | 2-Pin Right Angle / Vertical | Reversible polarized battery plug |
| **Solar Terminal** | **2-Pin Screw Terminal** | 5.08mm Pitch (e.g. Phoenix MKDS) | Heavy-duty terminal block for solar panel wire leads |
| **Header Sockets** | **2x 7-Pin Female Headers** | 2.54mm Pitch | Socketed header mounting for XIAO ESP32-C3 board |

---

### Key Features
* 🔋 **Ultra-Low-Power Deep Sleep:** Sleeps for 30 minutes (configurable) and completely cuts SHT31 sensor power via pin `D1` during sleep (0 µA standby).
* 📡 **Dual Telemetry Transmission:** Simultaneously broadcasts weather telemetry via **UDP Broadcast (Port 8888)** to all local network devices AND posts via **HTTP/HTTPS POST** to a central dashboard server.
* 📶 **WiFiManager Integration:** Easy captive portal configuration (`XIAO-C3-Solar-AP`) for credentials and API URLs without re-flashing.
* ⚡ **Precision Battery ADC:** 16-sample oversampling for battery voltage monitoring via 1:2 voltage divider on pin `A2` (0% to 100% mapping for 3.3V - 4.15V Li-Ion cells).
* 🌡️ **Dew Point Calculation:** Calculates exact dew point temperature directly on chip using Magnus-Tetens formula.
* 🛠️ **Maintenance Mode:** Pulling Pin `D3` to `GND` keeps the ESP32 awake indefinitely for debugging, serial monitoring, and OTA updates.
* 🔄 **Remote Offset Calibration:** Server HTTP response can remotely adjust the temperature offset and persist it to EEPROM.

---

## ⚡ Quick Flash / Schnelles Flashen (No Coding Needed / Ohne Installation)

### 🚀 1-Click Browser Flash (Empfohlen)
Du musst nichts kompilieren oder installieren! Lade einfach die fertige Firmware aus den [Releases](https://github.com/Daddelgreis74/esp32c3-solar-weather-sensor/releases) herunter:

1. Lade **`esp32c3-solar-weather-sensor-v1.0.0-factory.bin`** aus dem neuesten Release herunter.
2. Öffne einen Web-Flasher im Chrome oder Edge Browser:
   * 👉 **[web.esphome.io](https://web.esphome.io)** oder **[Adafruit WebSerial ESPTool](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/)**
3. Verbinde den Seeed Studio XIAO ESP32-C3 per USB-Kabel mit dem PC.
4. Klicke auf **Connect**, wähle den COM-Port aus und flashe die Datei `esp32c3-solar-weather-sensor-v1.0.0-factory.bin` bei Offset `0x0`.
5. Nach dem Flashen öffnet der ESP32 automatisch den Hotspot **`XIAO-C3-Solar-AP`**. Verbinde dich mit dem Smartphone und gib dein WLAN & deine Dashboard-URL ein – fertig!

---

## 🛠️ Build from Source / Aus Quellcode kompilieren (PlatformIO)

### 1. Requirements
* [PlatformIO IDE](https://platformio.org/) (VSCode extension or CLI)
* Seeed Studio XIAO ESP32-C3 board
* Sensirion SHT31 Temperature & Humidity Sensor (I2C 0x44)
* 3.7V Li-Ion / LiPo Battery + 5V Solar Panel + SOD-123 Schottky Diode

### 2. Clone & Configuration
```bash
git clone https://github.com/Daddelgreis74/esp32c3-solar-weather-sensor.git
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
