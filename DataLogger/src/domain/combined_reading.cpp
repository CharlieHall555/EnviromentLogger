#include "domain/combined_reading.h"

#include "sensors/pm_sensor.h"
#include "sensors/temp_sensor.h"

#include "debug.h"

void dumpReading(const CombinedReading& combined)
{
    debug::clearBufferLog();
    debug::addToLog("temp", combined.temp);
    debug::addToLog("hum", combined.hum);
    debug::addToLog("time", combined.time);
    debug::addToLog("pm1_0", combined._1);
    debug::addToLog("pm2_5", combined._2_5);
    debug::addToLog("pm10", combined._10);
    debug::commitLog("Combined-Reading", debug::Severity::INFO);
}

CombinedReading constructCombined(const TempReading& tempReading, const PMReading& pmReading , int time)
{
    CombinedReading reading;
    reading.temp = tempReading.temp;
    reading.hum  = tempReading.hum;
    reading._1   = pmReading._1;
    reading._2_5 = pmReading._2_5;
    reading._10  = pmReading._10;
    reading.time = time;
    return reading;
}