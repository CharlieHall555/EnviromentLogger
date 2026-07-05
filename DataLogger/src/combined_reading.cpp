#include "combined_reading.h"
#include "logger.h"

void dumpReading(CombinedReading combined)
{
    LOG("-- CombinedReading --");
    LOG("temp: " + String(combined.temp, 2));
    LOG("hum: " + String(combined.hum, 2));
    LOG("time: " + String(combined.time));
    LOG("PM1.0 :" + String(combined._1));
    LOG("PM2.5 :" + String(combined._2_5));
    LOG("PM10: " + String(combined._10));
}

CombinedReading constructCombined(TempReading tempReading, PMReading pmReading)
{
    CombinedReading reading;
    reading.temp = tempReading.temp;
    reading.hum  = tempReading.hum;
    reading.time = tempReading.time;
    reading._1   = pmReading._1;
    reading._2_5 = pmReading._2_5;
    reading._10  = pmReading._10;
    return reading;
}