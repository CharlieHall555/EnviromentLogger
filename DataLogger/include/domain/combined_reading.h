#pragma once

struct TempReading;
struct PMReading;

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