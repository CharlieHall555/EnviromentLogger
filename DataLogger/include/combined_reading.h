#pragma once

#include "temp_logger.h"
#include "pm_logger.h"

struct CombinedReading {

    float temp;
    float hum;
    int time = 0;
    int _1;
    int _2_5;
    int _10;

};

CombinedReading constructCombined(TempReading tempReading , PMReading pmReading);
void dumpReading(CombinedReading reading);