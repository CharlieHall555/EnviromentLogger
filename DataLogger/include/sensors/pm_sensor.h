#pragma once
#include <Arduino.h>
#include <optional>
#include "domain/combined_reading.h"

struct PMReading {
    int _1;
    int _2_5;
    int _10;
};

enum PMReadingQuality {
    NORMAL,
    ELEVATED,
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
        std::optional<PMReading> takeAverageReading(int samples, int delay);
        static bool readingSanityCheck(const PMReading &reading);
        static PMReadingQuality getReadingQuality(const PMReading &reading);
        static PMReadingQuality getReadingQuality(const CombinedReading &reading);

    private:    
        HardwareSerial pmsSerial; // UART2
        int startTime = 0;
        int failedAttempts = 0;
        int retryTime = 250;
        static constexpr int MAX_RETRIES = 5;
       
};
