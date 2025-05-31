#include "sensor_manager.h"

// Static heat index calculation
float ClimateManager::calculateHeatIndex(float temperature, float humidity) {
  if (temperature < 27.0) {
    return temperature; // Heat index only applies at higher temperatures
  }
  
  float hi = -8.78469475556 + 
             1.61139411 * temperature + 
             2.33854883889 * humidity + 
             -0.14611605 * temperature * humidity + 
             -0.012308094 * temperature * temperature + 
             -0.0164248277778 * humidity * humidity + 
             0.002211732 * temperature * temperature * humidity + 
             0.00072546 * temperature * humidity * humidity + 
             -0.000003582 * temperature * temperature * humidity * humidity;
  
  return hi;
}

#if SENSOR_TYPE == SENSOR_TYPE_DHT11 || SENSOR_TYPE == SENSOR_TYPE_DHT22
#include <DHT.h>
#include <DHT_U.h>

class DHTClimateManager : public ClimateManager {
private:
  DHT_Unified dht;
  sensor_t temperature_sensor;
  sensor_t humidity_sensor;
  
public:
  DHTClimateManager() : dht(DHT_PIN, DHT_TYPE) {}
  
  bool begin() override {
    dht.begin();
    
    // Get sensor details
    dht.temperature().getSensor(&temperature_sensor);
    dht.humidity().getSensor(&humidity_sensor);
    
    Serial.println("DHT Unified Sensor initialized");
    delay(2000); // DHT sensors need time to stabilize
    
    // Test sensor functionality
    sensors_event_t event;
    if (!dht.temperature().getEvent(&event)) {
      Serial.println("Error: DHT temperature sensor not responding");
      return false;
    }
    
    return true;
  }
  
  bool getTemperatureEvent(sensors_event_t* event) override {
    return dht.temperature().getEvent(event);
  }
  
  bool getHumidityEvent(sensors_event_t* event) override {
    return dht.humidity().getEvent(event);
  }
  
  void getTemperatureSensor(sensor_t* sensor) override {
    *sensor = temperature_sensor;
  }
  
  void getHumiditySensor(sensor_t* sensor) override {
    *sensor = humidity_sensor;
  }
  
  String getSensorName() override {
#if SENSOR_TYPE == SENSOR_TYPE_DHT11
    return "DHT11";
#else
    return "DHT22";
#endif
  }
  
  void printSensorInfo() override {
    Serial.println("=== DHT Sensor Information ===");
    Serial.print("Sensor Name: "); Serial.println(getSensorName());
    Serial.print("Pin: "); Serial.println(DHT_PIN);
    
    Serial.println("\n--- Temperature Sensor ---");
    Serial.print("Name: "); Serial.println(temperature_sensor.name);
    Serial.print("Version: "); Serial.println(temperature_sensor.version);
    Serial.print("ID: "); Serial.println(temperature_sensor.sensor_id);
    Serial.print("Type: "); Serial.println(temperature_sensor.type);
    Serial.print("Max Value: "); Serial.print(temperature_sensor.max_value); Serial.println(" °C");
    Serial.print("Min Value: "); Serial.print(temperature_sensor.min_value); Serial.println(" °C");
    Serial.print("Resolution: "); Serial.print(temperature_sensor.resolution); Serial.println(" °C");
    
    Serial.println("\n--- Humidity Sensor ---");
    Serial.print("Name: "); Serial.println(humidity_sensor.name);
    Serial.print("Version: "); Serial.println(humidity_sensor.version);
    Serial.print("ID: "); Serial.println(humidity_sensor.sensor_id);
    Serial.print("Type: "); Serial.println(humidity_sensor.type);
    Serial.print("Max Value: "); Serial.print(humidity_sensor.max_value); Serial.println(" %");
    Serial.print("Min Value: "); Serial.print(humidity_sensor.min_value); Serial.println(" %");
    Serial.print("Resolution: "); Serial.print(humidity_sensor.resolution); Serial.println(" %");
    Serial.println("==============================");
  }
};

#elif SENSOR_TYPE == SENSOR_TYPE_SHT41

#include <Wire.h>
#include <Adafruit_SHT4x.h>

class SHT41ClimateManager : public ClimateManager {
private:
  Adafruit_SHT4x sht4x;
  sensor_t temperature_sensor;
  sensor_t humidity_sensor;
  
public:
  SHT41ClimateManager() {}
  
  bool begin() override {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    
    if (!sht4x.begin()) {
      Serial.println("Couldn't find SHT4x sensor!");
      return false;
    }
    
    Serial.print("Found SHT4x sensor with serial number 0x");
    Serial.println(sht4x.readSerial(), HEX);
    
    // Configure sensor
    sht4x.setPrecision(SHT4X_HIGH_PRECISION);
    sht4x.setHeater(SHT4X_NO_HEATER);
    
    // Get sensor details
    sht4x.temperature().getSensor(&temperature_sensor);
    sht4x.humidity().getSensor(&humidity_sensor);
    
    Serial.println("SHT41 Unified Sensor initialized");
    return true;
  }
  
  bool getTemperatureEvent(sensors_event_t* event) override {
    sensors_event_t humidity_event; // Required by SHT4x getEvent
    return sht4x.getEvent(event, &humidity_event);
  }
  
  bool getHumidityEvent(sensors_event_t* event) override {
    sensors_event_t temp_event; // Required by SHT4x getEvent
    return sht4x.getEvent(&temp_event, event);
  }
  
  void getTemperatureSensor(sensor_t* sensor) override {
    *sensor = temperature_sensor;
  }
  
  void getHumiditySensor(sensor_t* sensor) override {
    *sensor = humidity_sensor;
  }
  
  String getSensorName() override {
    return "SHT41";
  }
  
  void printSensorInfo() override {
    Serial.println("=== SHT41 Sensor Information ===");
    Serial.print("Sensor Name: "); Serial.println(getSensorName());
    Serial.print("SDA Pin: "); Serial.println(I2C_SDA_PIN);
    Serial.print("SCL Pin: "); Serial.println(I2C_SCL_PIN);
    Serial.println("Protocol: I2C");
    Serial.println("Precision: High");
    Serial.println("Heater: Disabled");
    
    Serial.println("\n--- Temperature Sensor ---");
    Serial.print("Name: "); Serial.println(temperature_sensor.name);
    Serial.print("Version: "); Serial.println(temperature_sensor.version);
    Serial.print("ID: "); Serial.println(temperature_sensor.sensor_id);
    Serial.print("Type: "); Serial.println(temperature_sensor.type);
    Serial.print("Max Value: "); Serial.print(temperature_sensor.max_value); Serial.println(" °C");
    Serial.print("Min Value: "); Serial.print(temperature_sensor.min_value); Serial.println(" °C");
    Serial.print("Resolution: "); Serial.print(temperature_sensor.resolution); Serial.println(" °C");
    
    Serial.println("\n--- Humidity Sensor ---");
    Serial.print("Name: "); Serial.println(humidity_sensor.name);
    Serial.print("Version: "); Serial.println(humidity_sensor.version);
    Serial.print("ID: "); Serial.println(humidity_sensor.sensor_id);
    Serial.print("Type: "); Serial.println(humidity_sensor.type);
    Serial.print("Max Value: "); Serial.print(humidity_sensor.max_value); Serial.println(" %");
    Serial.print("Min Value: "); Serial.print(humidity_sensor.min_value); Serial.println(" %");
    Serial.print("Resolution: "); Serial.print(humidity_sensor.resolution); Serial.println(" %");
    Serial.println("=================================");
  }
};

#endif

// Factory function implementation
ClimateManager* createClimateSensor() {
#if SENSOR_TYPE == SENSOR_TYPE_DHT11 || SENSOR_TYPE == SENSOR_TYPE_DHT22
  return new DHTClimateManager();
#elif SENSOR_TYPE == SENSOR_TYPE_SHT41
  return new SHT41ClimateManager();
#else
  #error "Unsupported sensor type selected"
#endif
}
