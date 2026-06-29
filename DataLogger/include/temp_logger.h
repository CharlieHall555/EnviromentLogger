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
        bool sensorPresent();
        bool ensureReady();
        void connectToSensor();    
    public:
        TempSensor();
        bool isReady();
        bool startSensor();
        std::optional<TempReading> takeReading();


};

