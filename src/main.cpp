#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "config.h"
#include "climate_manager.h"
#include "homekit_manager.h"
#include "blynk_manager.h"
#include "power_manager.h"

// Climate sensor instance using Unified Sensor interface
ClimateManager* climateSensor = nullptr;

#if HOMEKIT_ENABLED
HomeKitManager homekit;
#endif

#if BLYNK_ENABLED
BlynkManager blynkManager;
#endif

// Timing variables
unsigned long previousMillis = 0;
const long interval = SENSOR_READ_INTERVAL;

// Function declarations
void initializeSystem();
void performQuickSensorRead();
void performSensorReading();

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);    // Give serial time to stabilize
  Serial.flush(); // Clear any garbage in buffer

  // Initialize power management system
  PowerManager::begin();

  // Check if waking from deep sleep - if so, perform quick operations
  if (PowerManager::isWakeupFromDeepSleep()) {
    Serial.println("Waking from deep sleep - performing quick sensor read...");
    performQuickSensorRead();
    return; // Skip normal initialization for quick wake cycle
  }

  // Normal boot initialization
  initializeSystem();
}

void initializeSystem() {
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

#if BLYNK_ENABLED
  if (WiFiManager::isConnected()) {
    blynkManager.begin();
    // Send initial status
    blynkManager.sendStatus(climateSensor->getSensorName(), true);
  }
#endif

  Serial.println("✓ System ready! Reading sensors every 60 seconds...");
#if SERIAL_DEBUG_VERBOSE
  delay(2000);
#else
  delay(1000);
#endif
}

void performQuickSensorRead() {
  // Initialize sensor for quick read
  climateSensor = createClimateSensor();
  if (!climateSensor || !climateSensor->begin()) {
    Serial.println("✗ Quick sensor initialization failed!");
    PowerManager::enterDeepSleep();
    return;
  }

  // Allow sensor to stabilize
  delay(SENSOR_STABILIZATION_DELAY);

  // Connect to WiFi quickly
  WiFi.begin();
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < 10000) { // 10 second timeout
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✓ WiFi connected for quick read");

    // Read sensor data
    sensors_event_t tempEvent, humidityEvent;
    if (climateSensor->getTemperatureEvent(&tempEvent) && 
        climateSensor->getHumidityEvent(&humidityEvent) &&
        !isnan(tempEvent.temperature) && !isnan(humidityEvent.relative_humidity)) {
      
      float temperature = tempEvent.temperature;
      float humidity = humidityEvent.relative_humidity;
      float heatIndex = ClimateManager::calculateHeatIndex(temperature, humidity);

      Serial.print("Quick read - Temp: ");
      Serial.print(temperature, 1);
      Serial.print("°C, Humidity: ");
      Serial.print(humidity, 1);
      Serial.println("%");

#if BLYNK_ENABLED
      // Send data to Blynk quickly
      blynkManager.begin();
      if (blynkManager.isConnected()) {
        blynkManager.sendSensorData(temperature, humidity, heatIndex);
        blynkManager.sendStatus(climateSensor->getSensorName(), true);
        Serial.println("✓ Data sent to Blynk");
      }
#endif
    } else {
      Serial.println("✗ Quick sensor read failed");
    }
  } else {
    Serial.println("✗ WiFi connection failed for quick read");
  }

  // Enter deep sleep immediately after quick operations
  Serial.println("Quick operations complete - returning to deep sleep");
  PowerManager::enterDeepSleep();
}

void loop() {
  // Check if we should enter deep sleep
  if (PowerManager::shouldEnterDeepSleep()) {
    Serial.println("Operation timeout reached or deep sleep scheduled");
    PowerManager::enterDeepSleep();
    return;
  }

  unsigned long currentMillis = millis();

#if HOMEKIT_ENABLED
  // HomeSpan must be polled regularly
  homekit.poll();
#endif

#if BLYNK_ENABLED
  // Blynk must be run regularly
  blynkManager.run();
#endif

  // Check WiFi status every 60 seconds
  static unsigned long lastWiFiCheck = 0;
  if (currentMillis - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    WiFiManager::checkStatus();
    lastWiFiCheck = currentMillis;
  }

  // Read sensor every 60 seconds (only in normal mode, not during quick wake)
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    performSensorReading();

    // Schedule deep sleep after successful sensor reading if enabled
    if (PowerManager::isDeepSleepEnabled()) {
      PowerManager::scheduleDeepSleep();
    }
  }

  if (!PowerManager::isDeepSleepEnabled()) {
    delay(1000);  // 1000ms light sleep - good balance of responsiveness and power savings
  }
}

void performSensorReading() {
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
      
#if BLYNK_ENABLED
      // Send error status to Blynk
      if (WiFiManager::isConnected() && blynkManager.isConnected()) {
        blynkManager.sendStatus(climateSensor->getSensorName(), false);
      }
#endif
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

#if BLYNK_ENABLED
    // Send sensor data to Blynk
    if (WiFiManager::isConnected() && blynkManager.isConnected()) {
      blynkManager.sendSensorData(temperature, humidity, heatIndex);
      blynkManager.sendStatus(climateSensor->getSensorName(), true);
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

#if BLYNK_ENABLED
    Serial.print("Blynk Status: ");
    Serial.println((WiFiManager::isConnected() && blynkManager.isConnected()) ? "Connected" : "Disconnected");
#endif

    Serial.print("Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.println("======================");
#endif
}