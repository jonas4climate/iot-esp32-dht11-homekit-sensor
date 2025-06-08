# IoT Smart Sensor - ESP32 Climate Sensor with HomeKit & Blynk Support

A modular climate sensor system for ESP32 that supports multiple sensor types and integrates with Apple HomeKit and Blynk IoT platform.

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
   - **For Blynk**: Set your Blynk Template ID and Auth Token (see Blynk Setup below)
4. Deploy via PlatformIO: Build + Upload
5. Monitor via Serial Monitor

## Blynk Setup

1. **Create Blynk Account**: Sign up at [blynk.cloud](https://blynk.cloud)
2. **Create New Template**:
   - Go to Developer Zone → My Templates
   - Click "New Template"
   - Name: "ESP32 Climate Monitor"
   - Hardware: "ESP32"
   - Connection Type: "WiFi"
3. **Configure Datastreams**:
   - V1: Temperature (°C) - Min: -40, Max: 85
   - V2: Humidity (%) - Min: 0, Max: 100  
   - V3: Heat Index (°C) - Min: -40, Max: 85
   - V4: Status (String)
4. **Get Credentials**:
   - Template ID: Copy from template settings
   - Auth Token: Create a new device and copy the token
5. **Update config.h** with your credentials
6. **Create Blynk App Dashboard**:
   - Add Gauge widgets for V1, V2, V3
   - Add Label widget for V4 (status)
   - Configure update intervals (1-60 seconds)

## Architecture

This project uses a modular architecture for easy sensor extension:

### Core Components:
- **`ClimateManager`** - Abstract interface using Adafruit Unified Sensor standard
- **`WiFiManager`** - Handles WiFi connectivity and reconnection
- **`HomeKitManager`** - Manages HomeSpan integration and HomeKit services
- **`BlynkManager`** - Handles Blynk IoT platform integration and data streaming
- **Factory Pattern** - `createClimateSensor()` instantiates the correct sensor type

### Adding New Sensors:
1. Add sensor type constant in `config.h`
2. Create new sensor class inheriting from `ClimateManager`
3. Implement required methods: `begin()`, `getTemperatureEvent()`, `getHumidityEvent()`, `getTemperatureSensor()`, `getHumiditySensor()`, `getSensorName()`, `printSensorInfo()`
4. Add case in factory function `createClimateSensor()`
5. Update library dependencies in `platformio.ini`

## Shared Components:
- WiFi connection management and reconnection logic
- HomeKit service definitions (Temperature + Humidity sensors)
- Blynk IoT platform integration with real-time data streaming
- Unified Sensor event handling with proper metadata
- Heat index calculation using sensor events
- Enhanced debugging with sensor metadata (resolution, min/max values, etc.)
- Configuration management system

## Unified Sensor Benefits:
- **Industry Standard**: Uses Adafruit Unified Sensor library standards
- **Rich Metadata**: Each sensor provides detailed specifications and capabilities  
- **Event-Based**: Uses `sensors_event_t` with timestamps and proper data types
- **Extensible**: Easy to add new sensors that follow the same interface
- **Ecosystem Compatible**: Works with other Adafruit sensor libraries and frameworks

## IoT Platform Integration:
- **Apple HomeKit**: Native integration for iOS/macOS smart home control
- **Blynk IoT**: Real-time monitoring, alerts, and remote access via mobile app
- **Dual Platform Support**: Both platforms work simultaneously
- **Cloud & Local**: HomeKit works locally, Blynk provides cloud-based monitoring