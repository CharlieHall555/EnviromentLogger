#pragma once
#include <optional>
#include <queue>
#include <Adafruit_BME280.h>

struct TempReading {

    float temp;
    float hum;
    int time = 0;

};

bool readingSanityCheck(TempReading reading);

class TempSensor{

    private:    
        Adafruit_BME280 sensor;
    public:
        TempSensor();
        bool startSensor();
        bool ensureReady();
        bool sensorPresent();
        std::optional<TempReading> takeReading();
};

