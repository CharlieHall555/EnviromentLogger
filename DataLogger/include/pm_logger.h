#pragma once
#include <Arduino.h>
#include <optional>

struct PMReading {

    int _1;
    int _2_5;
    int _10;

};

bool readingSanityCheck(PMReading reading);

class PMSensor{

    private:    
        HardwareSerial pmsSerial; // UART2
    public:
        PMSensor();
        bool sensorPresent();
        bool startSensor();
        bool ensureReady();
        std::optional<PMReading> takeReading();
};
