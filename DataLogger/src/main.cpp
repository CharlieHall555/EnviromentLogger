#include <Arduino.h>
#include <system.h>
#include <debug.h>

LoggerSystem loggerSystem;

void setup()
{
    Serial.begin(115200);
    loggerSystem.begin();
}

void loop()
{
    
}