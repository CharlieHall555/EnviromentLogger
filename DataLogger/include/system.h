#pragma once
#include "networking/wifi_handler.h"
#include "sensors/pm_sensor.h"
#include "sensors/temp_sensor.h"
#include "sleep_safe_queue.hpp"
#include "domain/combined_reading.h"


struct InitFlags
{
    bool wifi;
    bool tempSensor;
    bool pmSensor;
    bool timing;
};

struct SystemState
{

    InitFlags initFlags;
    bool wifiOk;
    bool tempSensorOk;
    bool pmSensorOk;
    int systemTime;
};

class LoggerSystem
{
    private:
        SystemState systemState;

        //Components
        PMSensor pmSensor;
        WiFiHandler wifiHandler;
        TempSensor tempSensor;

        void flushDebugLog();
        void pushNewReading(CombinedReading& reading);
        void logSystemState();
        void startupRoutine();

        SleepSafeQueue<CombinedReading , 10> savedReadingQueue;
        const int DEEP_SLEEP_TIME = 60 * 1000 * 1000;// microseconds (us);
    public:
        LoggerSystem();
        bool takeReading(CombinedReading &reading);
        void begin();
};