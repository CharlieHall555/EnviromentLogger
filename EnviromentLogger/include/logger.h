
#include <Arduino.h>

#ifndef DEBUG
  #define DEBUG 0
#endif

#ifdef DEBUG 
  #define LOG(x) Serial.println(x)
#else
  #define LOG(x) Serial.println(x)
#endif