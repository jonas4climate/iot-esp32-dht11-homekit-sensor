#include "blynk_manager.h"

#if BLYNK_ENABLED

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial

#include <BlynkSimpleEsp32.h>

BlynkManager* blynkManagerInstance = nullptr;

// Blynk callbacks
BLYNK_CONNECTED() {
  if (blynkManagerInstance) {
    BlynkManager::onConnected();
  }
}

BLYNK_DISCONNECTED() {
  if (blynkManagerInstance) {
    BlynkManager::onDisconnected();
  }
}

// Blynk App requests for current values
BLYNK_READ(BLYNK_VIRTUAL_PIN_TEMP) {
  // This will be called when Blynk app requests temperature value
  // The actual value will be sent via sendSensorData()
}

BLYNK_READ(BLYNK_VIRTUAL_PIN_HUMIDITY) {
  // This will be called when Blynk app requests humidity value
  // The actual value will be sent via sendSensorData()
}

BLYNK_READ(BLYNK_VIRTUAL_PIN_HEAT_INDEX) {
  // This will be called when Blynk app requests heat index value
  // The actual value will be sent via sendSensorData()
}

BLYNK_READ(BLYNK_VIRTUAL_PIN_STATUS) {
  // This will be called when Blynk app requests status
  // The actual status will be sent via sendStatus()
}

// BlynkManager Implementation
BlynkManager::BlynkManager() : initialized(false), lastConnectionCheck(0) {
  blynkManagerInstance = this;
}

bool BlynkManager::begin() {
  if (!WiFi.isConnected()) {
    Serial.println("✗ Blynk disabled - WiFi required");
    return false;
  }

  Serial.println("✓ Initializing Blynk...");
  
  // Connect to Blynk server using the auth token from config
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
  
  // Check if connected
  if (Blynk.connected()) {
    Serial.println("✓ Connected to Blynk server!");
    initialized = true;
    
#if SERIAL_DEBUG_VERBOSE
    Serial.print("Template ID: ");
    Serial.println("TMPL407-uPLKj");
    Serial.print("Template Name: ");
    Serial.println("ESP32 Climate Monitor");
    Serial.println("Virtual Pins:");
    Serial.print("  Temperature: V");
    Serial.println(BLYNK_VIRTUAL_PIN_TEMP);
    Serial.print("  Humidity: V");
    Serial.println(BLYNK_VIRTUAL_PIN_HUMIDITY);
    Serial.print("  Heat Index: V");
    Serial.println(BLYNK_VIRTUAL_PIN_HEAT_INDEX);
    Serial.print("  Status: V");
    Serial.println(BLYNK_VIRTUAL_PIN_STATUS);
#endif
    
    return true;
  } else {
    Serial.println("✗ Failed to connect to Blynk server");
    Serial.println("Check your Auth Token and internet connection");
    return false;
  }
}

void BlynkManager::run() {
  if (initialized && WiFi.isConnected()) {
    Blynk.run();
    
    // Periodic connection check
    unsigned long currentMillis = millis();
    if (currentMillis - lastConnectionCheck >= CONNECTION_CHECK_INTERVAL) {
      checkConnection();
      lastConnectionCheck = currentMillis;
    }
  }
}

void BlynkManager::sendSensorData(float temperature, float humidity, float heatIndex) {
  if (initialized && isConnected()) {
    // Send temperature
    Blynk.virtualWrite(BLYNK_VIRTUAL_PIN_TEMP, temperature);
    
    // Send humidity  
    Blynk.virtualWrite(BLYNK_VIRTUAL_PIN_HUMIDITY, humidity);
    
    // Send heat index
    Blynk.virtualWrite(BLYNK_VIRTUAL_PIN_HEAT_INDEX, heatIndex);
    
#if SERIAL_DEBUG_VERBOSE
    Serial.println("📱 Data sent to Blynk");
    Serial.print("  Temperature: ");
    Serial.print(temperature, 1);
    Serial.println("°C");
    Serial.print("  Humidity: ");
    Serial.print(humidity, 1);
    Serial.println("%");
    Serial.print("  Heat Index: ");
    Serial.print(heatIndex, 1);
    Serial.println("°C");
#endif
  }
}

void BlynkManager::sendStatus(const String& sensorName, bool isOnline) {
  if (initialized && isConnected()) {
    String status = isOnline ? "Online" : "Offline";
    Blynk.virtualWrite(BLYNK_VIRTUAL_PIN_STATUS, status);
    
#if SERIAL_DEBUG_VERBOSE
    Serial.print("📱 Status sent to Blynk: ");
    Serial.println(status);
#endif
  }
}

bool BlynkManager::isConnected() {
  return initialized && Blynk.connected();
}

void BlynkManager::checkConnection() {
  if (initialized && WiFi.isConnected() && !Blynk.connected()) {
#if SERIAL_DEBUG_VERBOSE
    Serial.println("⚠️  Blynk connection lost! Attempting to reconnect...");
#else
    Serial.println("Blynk reconnecting...");
#endif
    
    // Try to reconnect using begin() again
    Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASSWORD);
    
    if (Blynk.connected()) {
      Serial.println("✓ Blynk reconnected!");
    } else {
      Serial.println("✗ Blynk reconnection failed");
    }
  }
}

// Static callback implementations
void BlynkManager::onConnected() {
  Serial.println("✓ Blynk connected!");
}

void BlynkManager::onDisconnected() {
  Serial.println("⚠️  Blynk disconnected!");
}

#endif // BLYNK_ENABLED
