#pragma once

#include "domain/combined_reading.h"
#include "sensors/pm_sensor.h"
#include "sensors/temp_sensor.h"

struct CombinedReading {

    float temp;
    float hum;
    int time = 0;
    int _1;
    int _2_5;
    int _10;

};

CombinedReading constructCombined(const TempReading& tempReading, const PMReading& pmReading , int time);
void dumpReading(const CombinedReading& reading);