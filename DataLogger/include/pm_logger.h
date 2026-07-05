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
        bool sensorPresent();
        bool ensureReady();
        void connectToSensor();    
    public:
        PMSensor();
        bool isReady();
        bool startSensor();
        std::optional<PMReading> takeReading();
};
