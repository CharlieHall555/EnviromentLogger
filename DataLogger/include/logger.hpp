#pragma once
#include <Arduino.h>

#ifndef DEBUG
  #define DEBUG 0
#endif

#if DEBUG
  #define LOG(x) Serial.println(x)
  #define LOGF(...) Serial.printf(__VA_ARGS__)
#else
  #define LOG(x)
  #define LOGF(...)
#endif

inline void dumpMemoryState()
{
    LOG("[MemoryState]");
    LOGF("Free heap: %u\n", ESP.getFreeHeap());
    LOGF("Min free heap: %u\n", ESP.getMinFreeHeap());
    LOGF("Stack high water: %u\n", uxTaskGetStackHighWaterMark(NULL));
}
