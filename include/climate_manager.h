#ifndef CLIMATE_MANAGER_H
#define CLIMATE_MANAGER_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "config.h"

// Unified Sensor interface for climate sensors
class ClimateManager {
public:
  virtual ~ClimateManager() = default;
  
  // Core Unified Sensor interface
  virtual bool begin() = 0;
  virtual bool getTemperatureEvent(sensors_event_t* event) = 0;
  virtual bool getHumidityEvent(sensors_event_t* event) = 0;
  virtual void getTemperatureSensor(sensor_t* sensor) = 0;
  virtual void getHumiditySensor(sensor_t* sensor) = 0;
  
  // Convenience methods
  virtual String getSensorName() = 0;
  virtual void printSensorInfo() = 0;
  
  // Helper method to calculate heat index from events
  static float calculateHeatIndex(float temperature, float humidity);
};

// Factory function to create appropriate sensor
ClimateManager* createClimateSensor();

#endif
