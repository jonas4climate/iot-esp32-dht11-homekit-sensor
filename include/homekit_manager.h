#ifndef HOMEKIT_MANAGER_H
#define HOMEKIT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

#if HOMEKIT_ENABLED
#include "HomeSpan.h"

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

class HomeKitManager {
private:
  TemperatureSensor *tempSensor;
  HumiditySensor *humSensor;
  bool initialized;

public:
  HomeKitManager();
  bool begin(const String& deviceName);
  void poll();
  void updateSensorData(float temperature, float humidity);
  bool isInitialized() const { return initialized; }
};

#endif

// WiFi management functions
class WiFiManager {
public:
  static void connect();
  static void checkStatus();
  static bool isConnected() { return WiFi.status() == WL_CONNECTED; }
};

#endif
