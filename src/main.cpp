#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>

#include "config.h"

#if HOMEKIT_ENABLED
#include "HomeSpan.h"
#endif

// DHT11 sensor configuration
DHT dht(DHT_PIN, DHT_TYPE);

// Variables to store sensor readings
float temperature = 0.0;
float humidity = 0.0;
float heatIndex = 0.0;

// Timing variables
unsigned long previousMillis = 0;
const long interval = SENSOR_READ_INTERVAL;

#if HOMEKIT_ENABLED
// HomeSpan Temperature Sensor Service
struct TemperatureSensor : Service::TemperatureSensor {
  SpanCharacteristic *temp;

  TemperatureSensor() : Service::TemperatureSensor() {
    temp = new Characteristic::CurrentTemperature(20.0);
    temp->setRange(-40, 100);
  }

  void updateTemperature(float newTemp) { temp->setVal(newTemp); }
};

// HomeSpan Humidity Sensor Service
struct HumiditySensor : Service::HumiditySensor {
  SpanCharacteristic *humidity;

  HumiditySensor() : Service::HumiditySensor() {
    humidity = new Characteristic::CurrentRelativeHumidity(50.0);
    humidity->setRange(0, 100);
  }

  void updateHumidity(float newHumidity) { humidity->setVal(newHumidity); }
};

// Global pointers to HomeSpan services
TemperatureSensor *tempSensor = nullptr;
HumiditySensor *humSensor = nullptr;
#endif

// WiFi connection function
void connectToWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("✓ WiFi connected!");
#if SERIAL_DEBUG_VERBOSE
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
#endif
  } else {
    Serial.println();
    Serial.println("✗ WiFi connection failed!");
#if SERIAL_DEBUG_VERBOSE
    Serial.println("Continuing without WiFi connection...");
#endif
  }
  Serial.println();
}

// Function to check WiFi status
void checkWiFiStatus() {
  if (WiFi.status() != WL_CONNECTED) {
#if SERIAL_DEBUG_VERBOSE
    Serial.println("WiFi connection lost! Attempting to reconnect...");
#else
    Serial.println("WiFi reconnecting...");
#endif
    connectToWiFi();
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);    // Give serial time to stabilize
  Serial.flush(); // Clear any garbage in buffer

  Serial.println();
  Serial.println("ESP32 DHT11 Temperature and Humidity Sensor");
  Serial.println("===========================================");

  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT11 sensor initialized successfully!");

  // Connect to WiFi
  connectToWiFi();

#if HOMEKIT_ENABLED
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("✓ Initializing HomeSpan...");

    homeSpan.begin(Category::Sensors, HOMEKIT_DEVICE_NAME);

    // Set WiFi credentials for HomeSpan
    homeSpan.setWifiCredentials(WIFI_SSID, WIFI_PASSWORD);

    // Enable factory reset - hold down boot button (GPIO0) for 10+ seconds
    homeSpan.setControlPin(0); // Use GPIO0 (boot button) for factory reset

#if SERIAL_DEBUG_VERBOSE
    // Optional: Set custom network settings for setup mode
    homeSpan.setHostNameSuffix("-Climate");
    homeSpan.setApSSID("ESP32-Climate-Setup");
    homeSpan.setPairingCode(HOMEKIT_SETUP_CODE);
    homeSpan.setApPassword(HOMEKIT_PASSWORD);
    homeSpan.setApTimeout(300); // 5 minutes timeout for setup mode
#endif

    // Enable Over-The-Air updates
    homeSpan.enableOTA();

    new SpanAccessory();
    new Service::AccessoryInformation();
    new Characteristic::Name(HOMEKIT_DEVICE_NAME);
    new Characteristic::Manufacturer(HOMEKIT_DEVICE_MANUFACTURER);
    new Characteristic::SerialNumber(HOMEKIT_DEVICE_SERIAL);
    new Characteristic::Model(HOMEKIT_DEVICE_MODEL);
    new Characteristic::FirmwareRevision("1.0.0");
    new Characteristic::Identify();

    tempSensor = new TemperatureSensor();
    humSensor = new HumiditySensor();

    Serial.println("✓ HomeSpan ready!");
#if SERIAL_DEBUG_VERBOSE
    Serial.print("Setup code: ");
    Serial.println(HOMEKIT_SETUP_CODE);
    Serial.println("To factory reset: Hold BOOT button for 10+ seconds");
#endif
  } else {
    Serial.println("✗ HomeSpan disabled - WiFi required");
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
  homeSpan.poll();
#endif

  // Check WiFi status every 60 seconds
  static unsigned long lastWiFiCheck = 0;
  if (currentMillis - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    checkWiFiStatus();
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

    // Read humidity and temperature
    humidity = dht.readHumidity();
    temperature = dht.readTemperature(); // Celsius by default

    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println();
      Serial.println("ERROR: Failed to read from DHT sensor!");
      Serial.println("Check sensor wiring and connections.");
      return;
    }

    // Calculate heat index (feels like temperature)
    heatIndex = dht.computeHeatIndex(temperature, humidity,
                                     false); // false = Celsius

#if HOMEKIT_ENABLED
    // Update HomeSpan characteristics with new sensor values
    if (WiFi.status() == WL_CONNECTED && tempSensor && humSensor) {
      tempSensor->updateTemperature(temperature);
      humSensor->updateHumidity(humidity);
    }
#endif

    // Print readings to serial monitor
#if SERIAL_DEBUG_VERBOSE
    Serial.println();
    Serial.println("=== Sensor Reading ===");
#endif
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print("°C | Humidity: ");
    Serial.print(humidity);
    Serial.print("% | Heat Index: ");
    Serial.print(heatIndex);
    Serial.println("°C");

#if SERIAL_DEBUG_VERBOSE
    Serial.print("WiFi Status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected"
                                                 : "Disconnected");

#if HOMEKIT_ENABLED
    Serial.print("HomeKit Status: ");
    Serial.println((WiFi.status() == WL_CONNECTED && tempSensor && humSensor)
                       ? "Active"
                       : "Inactive");
#endif

    Serial.print("Uptime: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.println("======================");
#endif
  }
}