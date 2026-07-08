#pragma once
#include <Arduino.h>
#include <optional>

struct PMReading {

    int _1;
    int _2_5;
    int _10;

};


class PMSensor{

    public:
        PMSensor();
        bool setup();
        int timeActive();
        void wake();
        void sleep();
        bool isSendingData();
        bool startSensor();
        bool ensureReady();
        std::optional<PMReading> takeReading();
        static bool readingSanityCheck(const PMReading &reading);
    private:    
        HardwareSerial pmsSerial; // UART2
        int startTime = 0;
        int failedAttempts = 0;
        int retryTime = 250;
        static constexpr int MAX_RETRIES = 5;
       
};
