#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

#include <Arduino.h>
#include "config.h"

#if BLYNK_ENABLED
#include <WiFi.h>
// Forward declaration to avoid multiple inclusion of BlynkSimpleEsp32.h
class BlynkWifi;
extern BlynkWifi Blynk;

class BlynkManager {
private:
  bool initialized;
  unsigned long lastConnectionCheck;
  static const unsigned long CONNECTION_CHECK_INTERVAL = 30000; // 30 seconds

public:
  BlynkManager();
  bool begin();
  void run();
  void sendSensorData(float temperature, float humidity, float heatIndex);
  void sendStatus(const String& sensorName, bool isOnline);
  bool isConnected();
  void checkConnection();
  
  // Static callback functions for Blynk
  static void onConnected();
  static void onDisconnected();
};

#endif // BLYNK_ENABLED

#endif // BLYNK_MANAGER_H
