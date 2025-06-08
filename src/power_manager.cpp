// filepath: src/power_manager.cpp
#include "power_manager.h"
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <WiFi.h>

// Static member initialization
unsigned long PowerManager::wakeupTime = 0;
unsigned long PowerManager::operationStartTime = 0;
bool PowerManager::deepSleepScheduled = false;

void PowerManager::begin() {
  wakeupTime = millis();
  operationStartTime = millis();
  deepSleepScheduled = false;

#if DEEP_SLEEP_ENABLED
  // Print wakeup reason for debugging
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  Serial.println("=== Power Manager Initialized ===");
  Serial.print("Wake-up reason: ");
  Serial.println(getWakeupReason());
  
  // Configure timer wake-up source
  esp_sleep_enable_timer_wakeup(DEEP_SLEEP_DURATION * 1000000ULL); // Convert seconds to microseconds
  
  Serial.print("Deep sleep enabled - Duration: ");
  Serial.print(DEEP_SLEEP_DURATION);
  Serial.println(" seconds");
  
  // Configure wake-up sources
  Serial.println("Timer wake-up configured");
#else
  Serial.println("=== Power Manager Initialized ===");
  Serial.println("Deep sleep disabled");
#endif

  Serial.print("Operation timeout: ");
  Serial.print(DEEP_SLEEP_OPERATION_TIMEOUT);
  Serial.println(" seconds");
}

bool PowerManager::shouldEnterDeepSleep() {
#if !DEEP_SLEEP_ENABLED
  return false;
#endif

  return deepSleepScheduled || isOperationTimeoutReached();
}

void PowerManager::enterDeepSleep() {
#if !DEEP_SLEEP_ENABLED
  Serial.println("Deep sleep disabled in config");
  return;
#endif

  Serial.println("=== Entering Deep Sleep ===");
  printPowerStats();
  
  // Prepare for sleep
  // inline printing using %
  Serial.print("Sleeping for");
  Serial.print(DEEP_SLEEP_DURATION);
  Serial.println(" seconds...");
  
  // Flush serial output
  Serial.flush();
  
  // Disconnect WiFi to save power
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  
  // Disable Bluetooth
  esp_bt_controller_disable();
  
  // Enter deep sleep
  esp_deep_sleep_start();
}

bool PowerManager::isWakeupFromDeepSleep() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  return wakeup_reason == ESP_SLEEP_WAKEUP_TIMER || 
         wakeup_reason == ESP_SLEEP_WAKEUP_EXT0 || 
         wakeup_reason == ESP_SLEEP_WAKEUP_EXT1 ||
         wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD ||
         wakeup_reason == ESP_SLEEP_WAKEUP_ULP;
}

String PowerManager::getWakeupReason() {
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
  
  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      return "External signal using RTC_IO";
    case ESP_SLEEP_WAKEUP_EXT1:
      return "External signal using RTC_CNTL";
    case ESP_SLEEP_WAKEUP_TIMER:
      return "Timer";
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      return "Touchpad";
    case ESP_SLEEP_WAKEUP_ULP:
      return "ULP program";
    default:
      return "First boot or reset";
  }
}

void PowerManager::resetOperationTimer() {
  operationStartTime = millis();
  deepSleepScheduled = false;
}

bool PowerManager::isOperationTimeoutReached() {
#if !DEEP_SLEEP_ENABLED
  return false;
#endif

  return getOperationTime() >= (DEEP_SLEEP_OPERATION_TIMEOUT * 1000);
}

unsigned long PowerManager::getOperationTime() {
  return millis() - operationStartTime;
}

void PowerManager::scheduleDeepSleep() {
#if DEEP_SLEEP_ENABLED
  deepSleepScheduled = true;
  Serial.println("Deep sleep scheduled after current operations");
#endif
}

void PowerManager::cancelDeepSleep() {
  deepSleepScheduled = false;
  Serial.println("Deep sleep cancelled");
}

bool PowerManager::isDeepSleepEnabled() {
#if DEEP_SLEEP_ENABLED
  return true;
#else
  return false;
#endif
}

void PowerManager::printPowerStats() {
  Serial.println("=== Power Statistics ===");
  Serial.print("Total wake time: ");
  Serial.print(millis() - wakeupTime);
  Serial.println(" ms");
  
  Serial.print("Operation time: ");
  Serial.print(getOperationTime());
  Serial.println(" ms");
  
  Serial.print("Free heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  
  Serial.print("CPU frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
}
