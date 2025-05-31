# IoT Smart Sensor - ESP32 Climate Sensor with HomeKit Support

A modular climate sensor system for ESP32 that supports multiple sensor types and integrates with Apple HomeKit.

## Supported Sensors

- **DHT11** - Basic temperature and humidity sensor
- **DHT22** - Higher precision temperature and humidity sensor  
- **SHT41** - High-precision I2C temperature and humidity sensor

## Hardware Requirements

- ESP32 board (any should work)
- One of the supported sensors:
  - DHT11 or DHT22 temperature/humidity sensor + 3 jumper wires
  - SHT41 sensor + 4 jumper wires (I2C)
- Breadboard (optional but recommended)

## Configuration

Edit `include/config.h` to select your sensor type:

```cpp
// Select sensor type
#define SENSOR_TYPE SENSOR_TYPE_DHT11    // or SENSOR_TYPE_DHT22 or SENSOR_TYPE_SHT41

// Configure pins based on sensor type
#define DHT_PIN 4                        // For DHT11/DHT22
#define I2C_SDA_PIN 21                   // For SHT41
#define I2C_SCL_PIN 22                   // For SHT41
```

## Wiring

### DHT11/DHT22
- VCC → 3.3V
- GND → GND  
- DATA → GPIO 4 (configurable)

### SHT41
- VCC → 3.3V
- GND → GND
- SDA → GPIO 21 (configurable)
- SCL → GPIO 22 (configurable)

## Deploy

1. Install PlatformIO in VS Code
2. Open this project folder
3. Edit `include/config.h`:
   - Set your WiFi credentials
   - Choose your sensor type (`SENSOR_TYPE_DHT11`, `SENSOR_TYPE_DHT22`, or `SENSOR_TYPE_SHT41`)
   - Configure pins if needed
4. Deploy via PlatformIO: Build + Upload
5. Monitor via Serial Monitor

## Architecture

This project uses a modular architecture for easy sensor extension:

### Core Components:
- **`SensorManager`** - Abstract interface for all sensors
- **`WiFiManager`** - Handles WiFi connectivity and reconnection
- **`HomeKitManager`** - Manages HomeSpan integration and HomeKit services
- **Factory Pattern** - `createSensor()` instantiates the correct sensor type

### Adding New Sensors:
1. Add sensor type constant in `config.h`
2. Create new sensor class inheriting from `SensorManager`
3. Implement required methods: `begin()`, `readSensor()`, `getSensorName()`, `printDebugInfo()`
4. Add case in factory function `createSensor()`
5. Update library dependencies in `platformio.ini`

## Shared Components:
- WiFi connection management and reconnection logic
- HomeKit service definitions (Temperature + Humidity sensors)
- Serial output formatting and debug information
- Main loop structure with timing and error handling
- Configuration management system