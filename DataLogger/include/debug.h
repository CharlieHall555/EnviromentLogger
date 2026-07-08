#pragma once
#include <Arduino.h>
#include "sleep_safe_queue.hpp"
#include "json.hpp"
using json = nlohmann::json;

namespace debug {
    constexpr int DEBUG_MESSAGE_QUEUE = 3;
    constexpr size_t LOG_EVENT_MAX_LEN = 32;
    constexpr size_t LOG_MESSAGE_MAX_LEN = 256;

    extern json logBuffer;

    enum Severity {
        INFO,
        WARN,
        CRITICAL,
        STARTUP,
    };

    struct Config {
        Severity printThreshold = Severity::INFO;
        Severity remoteLogThreshold = Severity::INFO;
    };

    extern Config config;

    void setConfig(Config _config);

    void clearBufferLog();
    void addToLog(String key , String value);
    void addToLog(String key , const char* value);
    void addToLog(String key , int value);
    void addToLog(String key , float value);
    void addToLog(String key , double value);
    void addToLog(String key , bool value);
    void addToLog(String key , long value);
    void addToLog(String key , unsigned int value);
    void quickLog(const String& message);
    void enableRemoteLogging();
    void disableRemoteLogging();
    void setRemoteLoggingOverride(bool enabled);
    bool remoteLoggingOverrideEnabled();
    void commitLog(String event , Severity severity);
    void flushRemoteLogs();
    bool hasRemoteLog();
    int remoteLogCount();

    bool isHigherSeverity(Severity s1, Severity s2);
    const char* severityToString(Severity severity);

    struct Log{
        char event[LOG_EVENT_MAX_LEN];
        char message[LOG_MESSAGE_MAX_LEN];
        int loggedAt;
        Severity severity;
    };

    bool takeRemoteLog(Log& output);
    const SleepSafeQueue<Log, DEBUG_MESSAGE_QUEUE>& remoteLogs();

    // RTC queue storage is defined once in debug.cpp to avoid multiple definitions.
    extern RTC_DATA_ATTR SleepSafeQueue<Log , DEBUG_MESSAGE_QUEUE> toRemoteLog;
    
}