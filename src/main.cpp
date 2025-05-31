#include <Arduino.h>
#include <DHT.h>
#include <WiFi.h>
#include "config.h"

// DHT11 sensor configuration
DHT dht(DHT_PIN, DHT_TYPE);

// Variables to store sensor readings
float temperature = 0.0;
float humidity = 0.0;
float heatIndex = 0.0;

// Timing variables
unsigned long previousMillis = 0;
const long interval = SENSOR_READ_INTERVAL;

// WiFi connection function
void connectToWiFi() {
  Serial.println();
  Serial.print("Connecting to WiFi network: ");
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
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println();
    Serial.println("Failed to connect to WiFi!");
    Serial.println("Continuing without WiFi connection...");
  }
  Serial.println();
}

// Function to check WiFi status
void checkWiFiStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost! Attempting to reconnect...");
    connectToWiFi();
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println("ESP32 DHT11 Temperature and Humidity Sensor");
  Serial.println("===========================================");
  
  // Initialize DHT sensor
  dht.begin();
  Serial.println("DHT11 sensor initialized successfully!");
  
  // Connect to WiFi
  connectToWiFi();
  
  Serial.println("System ready! Starting sensor readings in 2 seconds...");
  delay(2000);
}

void loop() {
  unsigned long currentMillis = millis();
  
  // Check WiFi status every 30 seconds
  static unsigned long lastWiFiCheck = 0;
  if (currentMillis - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    checkWiFiStatus();
    lastWiFiCheck = currentMillis;
  }
  
  // Read sensor every 1 second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Read humidity and temperature
    humidity = dht.readHumidity();
    temperature = dht.readTemperature(); // Celsius by default
    
    // Check if readings are valid
    if (isnan(humidity) || isnan(temperature)) {
      Serial.println("ERROR: Failed to read from DHT sensor!");
      return;
    }
    
    // Calculate heat index (feels like temperature)
    heatIndex = dht.computeHeatIndex(temperature, humidity, false); // false = Celsius
    
    // Print readings to serial monitor
    Serial.println("----------------------------------------");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
    
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    
    Serial.print("Heat Index: ");
    Serial.print(heatIndex);
    Serial.println(" °C");
    
    Serial.print("WiFi Status: ");
    Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
    
    Serial.print("Timestamp: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.println("----------------------------------------");
  }
}