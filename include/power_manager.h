#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>
#include "config.h"

class PowerManager {
private:
  static unsigned long wakeupTime;
  static unsigned long operationStartTime;
  static bool deepSleepScheduled;
  
public:
  PowerManager();
  
  // Initialize power management
  static void begin();
  
  // Check if we should enter deep sleep
  static bool shouldEnterDeepSleep();
  
  // Enter deep sleep mode
  static void enterDeepSleep();
  
  // Check if this is a wakeup from deep sleep
  static bool isWakeupFromDeepSleep();
  
  // Get wakeup reason
  static String getWakeupReason();
  
  // Reset operation timer
  static void resetOperationTimer();
  
  // Check if operation timeout has been reached
  static bool isOperationTimeoutReached();
  
  // Get time since operation started
  static unsigned long getOperationTime();
  
  // Schedule deep sleep after current operations complete
  static void scheduleDeepSleep();
  
  // Cancel scheduled deep sleep
  static void cancelDeepSleep();
  
  // Check if deep sleep is enabled in configuration
  static bool isDeepSleepEnabled();
  
  // Print power statistics
  static void printPowerStats();
};

#endif // POWER_MANAGER_H
