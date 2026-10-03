#pragma once
#include <Arduino.h>

inline void debounce(void (*callback)(), unsigned long delayTime) {
  static unsigned long lastCallTime = 0;
  unsigned long currentTime = millis();
  if (currentTime - lastCallTime >= delayTime) {
    callback();
    lastCallTime = currentTime;
  }
}
