# IoT Smart Sensor - ESP32 DHT11 Temperature and Humidity Monitor

## Capabilities

- Reads temperature and humidity data from DHT11 sensor every 2 seconds
- Calculates heat index (feels-like temperature)
- Prints formatted sensor data to serial monitor with timestamps
- Handles sensor reading errors gracefully
- Real-time monitoring via USB serial connection

## Hardware Requirements

- ESP32 Devkit v1 (or compatible)
- DHT11 temperature and humidity sensor
- Jumper wires (3 wires needed)
- Breadboard (optional but recommended)

## Wiring Diagram

```
DHT11 Sensor    ESP32 DevKit v1
+----------+    +-------------+
| VCC      | -> | 3.3V        |
| DATA     | -> | GPIO4 (D4)  |
| GND      | -> | GND         |
+----------+    +-------------+
```

**Note:** DHT11 data pin connects to ESP32 pin GPIO4 (labeled as D4 on most boards)

## Software Features

- **Temperature Reading**: Celsius measurement with error handling
- **Humidity Reading**: Relative humidity percentage
- **Heat Index Calculation**: Computed "feels like" temperature
- **WiFi Connectivity**: Automatic connection with reconnection handling
- **HomeKit Integration**: Apple HomeKit compatible temperature and humidity sensors
- **Serial Output**: Formatted data output every second
- **Error Detection**: Validates sensor readings and reports failures

## Getting Started

### Prerequisites
- PlatformIO IDE or PlatformIO CLI
- USB cable for ESP32 programming and serial monitoring

### WiFi Configuration
1. Copy `include/config.h.template` to `include/config.h`
2. Edit `include/config.h` and update with your WiFi credentials:
   ```cpp
   #define WIFI_SSID "your_network_name"
   #define WIFI_PASSWORD "your_password"
   ```
3. The `config.h` file is ignored by git to keep your credentials secure

### HomeKit Configuration
HomeKit is enabled by default. To use it:
1. Make sure `HOMEKIT_ENABLED` is set to `true` in your config.h
2. Customize the device information in config.h:
   ```cpp
   #define HOMEKIT_DEVICE_NAME "ESP32 Climate Sensor"
   #define HOMEKIT_SETUP_CODE "111-22-333"  // 8-digit code for pairing
   ```
3. After uploading, use the Apple Home app to add the accessory using the setup code

To disable HomeKit, set `HOMEKIT_ENABLED` to `false` in config.h

### Installation
1. Clone or download this project
2. Configure WiFi credentials (see above)
3. Open in PlatformIO
4. Build and upload to your ESP32
5. Open Serial Monitor (115200 baud rate) to view readings

### Expected Output
```
ESP32 DHT11 Temperature and Humidity Sensor
===========================================
Sensor initialized successfully!
Starting readings in 2 seconds...
----------------------------------------
Temperature: 23.50 °C
Humidity: 60.20 %
Heat Index: 24.12 °C
Timestamp: 12 seconds
----------------------------------------
```

## Configuration

The sensor reading interval can be modified in `src/main.cpp`:
```cpp
const long interval = 2000; // Change to desired interval in milliseconds
```

## Troubleshooting

- **No readings/NaN values**: Check wiring connections
- **Erratic readings**: Ensure stable power supply (3.3V)
- **Upload issues**: Press and hold BOOT button on ESP32 during upload
- **Serial output not visible**: Verify baud rate is set to 115200