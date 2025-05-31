#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "config.h"
#include "sensor_manager.h"
#include "homekit_manager.h"

// Climate sensor instance using Unified Sensor interface
ClimateManager* climateSensor = nullptr;

#if HOMEKIT_ENABLED
HomeKitManager homekit;
#endif

// Timing variables
unsigned long previousMillis = 0;
const long interval = SENSOR_READ_INTERVAL;

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);    // Give serial time to stabilize
  Serial.flush(); // Clear any garbage in buffer

  Serial.println();
  Serial.println("ESP32 Climate Sensor with HomeKit Support");
  Serial.println("========================================");

  // Create and initialize climate sensor using factory pattern
  climateSensor = createClimateSensor();
  if (!climateSensor) {
    Serial.println("✗ Failed to create climate sensor instance!");
    return;
  }

  Serial.print("Initializing ");
  Serial.print(climateSensor->getSensorName());
  Serial.println(" sensor...");
  
  if (climateSensor->begin()) {
    Serial.println("✓ Sensor initialized successfully!");
    climateSensor->printSensorInfo();
  } else {
    Serial.println("✗ Sensor initialization failed!");
    return;
  }

  // Connect to WiFi
  WiFiManager::connect();

#if HOMEKIT_ENABLED
  if (WiFiManager::isConnected()) {
    String deviceName = String(HOMEKIT_DEVICE_NAME) + " (" + climateSensor->getSensorName() + ")";
    homekit.begin(deviceName);
  }
#endif

  Serial.println("✓ System ready! Reading sensors every 60 seconds...");
#if SERIAL_DEBUG_VERBOSE
  delay(2000);
#else
  delay(1000);
#endif
}

void loop() {
  unsigned long currentMillis = millis();

#if HOMEKIT_ENABLED
  // HomeSpan must be polled regularly
  homekit.poll();
#endif

  // Check WiFi status every 60 seconds
  static unsigned long lastWiFiCheck = 0;
  if (currentMillis - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    WiFiManager::checkStatus();
    lastWiFiCheck = currentMillis;
  }

  // Read sensor every 60 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Ensure serial is ready
    if (!Serial) {
      Serial.begin(SERIAL_BAUD_RATE);
      delay(100);
    }

    // Read sensor data using Unified Sensor interface
    sensors_event_t tempEvent, humidityEvent;
    
    bool tempValid = climateSensor->getTemperatureEvent(&tempEvent);
    bool humidityValid = climateSensor->getHumidityEvent(&humidityEvent);

    // Check if readings are valid
    if (!tempValid || !humidityValid || isnan(tempEvent.temperature) || isnan(humidityEvent.relative_humidity)) {
      Serial.println();
      Serial.print("ERROR: Failed to read from ");
      Serial.print(climateSensor->getSensorName());
      Serial.println(" sensor!");
      Serial.println("Check sensor wiring and connections.");
      return;
    }

    // Extract values from sensor events
    float temperature = tempEvent.temperature;
    float humidity = humidityEvent.relative_humidity;
    float heatIndex = ClimateManager::calculateHeatIndex(temperature, humidity);

#if HOMEKIT_ENABLED
    // Update HomeSpan characteristics with new sensor values
    if (WiFiManager::isConnected() && homekit.isInitialized()) {
      homekit.updateSensorData(temperature, humidity);
    }
#endif

    // Print readings to serial monitor
#if SERIAL_DEBUG_VERBOSE
    Serial.println();
    Serial.println("=== Sensor Reading ===");
#endif
    Serial.print("Temp: ");
    Serial.print(temperature, 1);
    Serial.print("°C | Humidity: ");
    Serial.print(humidity, 1);
    Serial.print("% | Heat Index: ");
    Serial.print(heatIndex, 1);
    Serial.println("°C");

#if SERIAL_DEBUG_VERBOSE
    Serial.print("Sensor: ");
    Serial.println(climateSensor->getSensorName());
    Serial.print("Timestamp: ");
    Serial.print(tempEvent.timestamp);
    Serial.println(" ms");
    Serial.print("WiFi Status: ");
    Serial.println(WiFiManager::isConnected() ? "Connected" : "Disconnected");

#if HOMEKIT_ENABLED
    Serial.print("HomeKit Status: ");
    Serial.println((WiFiManager::isConnected() && homekit.isInitialized()) ? "Active" : "Inactive");
#endif

    Serial.print("Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.println("======================");
#endif
  }
}