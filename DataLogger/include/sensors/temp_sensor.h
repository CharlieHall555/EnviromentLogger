#pragma once
#include <optional>
#include <queue>
#include <Adafruit_BME280.h>

struct TempReading {
    float temp;
    float hum;
};

class TempSensor{

    public:
        TempSensor();
        bool setup();
        bool startSensor();
        bool ensureReady();
        bool sensorPresent();
        std::optional<TempReading> takeReading();
        static bool readingSanityCheck(const TempReading &reading);
    private:    
        Adafruit_BME280 sensor;
        static constexpr int MAX_RETRIES = 5;
        
};

