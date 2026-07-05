#include "combinedReading.h"

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