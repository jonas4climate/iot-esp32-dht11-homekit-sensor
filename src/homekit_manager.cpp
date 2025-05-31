#include "homekit_manager.h"

// WiFi Manager Implementation
void WiFiManager::connect() {
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

void WiFiManager::checkStatus() {
  if (WiFi.status() != WL_CONNECTED) {
#if SERIAL_DEBUG_VERBOSE
    Serial.println("WiFi connection lost! Attempting to reconnect...");
#else
    Serial.println("WiFi reconnecting...");
#endif
    connect();
  }
}

#if HOMEKIT_ENABLED

// HomeKit Manager Implementation
HomeKitManager::HomeKitManager() : tempSensor(nullptr), humSensor(nullptr), initialized(false) {}

bool HomeKitManager::begin(const String& deviceName) {
  if (!WiFiManager::isConnected()) {
    Serial.println("✗ HomeKit disabled - WiFi required");
    return false;
  }

  Serial.println("✓ Initializing HomeSpan...");

  homeSpan.begin(Category::Sensors, deviceName.c_str());

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

  initialized = true;

  Serial.println("✓ HomeSpan ready!");
#if SERIAL_DEBUG_VERBOSE
  Serial.print("Setup code: ");
  Serial.println(HOMEKIT_SETUP_CODE);
  Serial.println("To factory reset: Hold BOOT button for 10+ seconds");
#endif

  return true;
}

void HomeKitManager::poll() {
  if (initialized) {
    homeSpan.poll();
  }
}

void HomeKitManager::updateSensorData(float temperature, float humidity) {
  if (initialized && WiFiManager::isConnected() && tempSensor && humSensor) {
    tempSensor->updateTemperature(temperature);
    humSensor->updateHumidity(humidity);
  }
}

#endif
