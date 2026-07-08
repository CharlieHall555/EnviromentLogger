#include "debug.h"
#include "json.hpp"
#include "timing.h"
#include "secrets.h"
#include "networking/request_handler.h"

json debug::logBuffer;
debug::Config debug::config{debug::Severity::INFO, debug::Severity::INFO};
RTC_DATA_ATTR SleepSafeQueue<debug::Log, debug::DEBUG_MESSAGE_QUEUE> debug::toRemoteLog;
static bool remoteOverrideEnabled = true;

static bool trySendRemoteLog(const debug::Log& log)
{
    String hardwareLogEndpoint = String(BASE_API_URL) + String(HARDWARE_LOG_ENDPOINT);

    json requestBody;
    requestBody["event"] = log.event;
    requestBody["severity"] = debug::severityToString(log.severity);
    if (json::accept(log.message))
    {
        requestBody["metadata"] = json::parse(log.message, nullptr, false);
    }
    else
    {
        requestBody["metadata"] = json::object({{"raw", log.message}});
    }
    requestBody["sent_at"] = log.loggedAt;

    requestHandler::postRequestParams requestParams;
    requestParams.URL = hardwareLogEndpoint;
    requestParams.token = String(API_TOKEN);
    requestParams.body = String(requestBody.dump().c_str());

    requestHandler::httpResponse response = requestHandler::postRequest(requestParams);
    return requestHandler::responseSuccess(response.code);
}

void debug::setConfig(Config _config)
{
    config = _config;
}

void debug::clearBufferLog()
{
    logBuffer.clear();
}

void debug::addToLog(String key, String value)
{
    logBuffer[key.c_str()] = value.c_str();
}

void debug::addToLog(String key, const char* value)
{
    addToLog(key, String(value));
}

void debug::addToLog(String key, int value)
{
    addToLog(key, String(value));
}

void debug::addToLog(String key, float value)
{
    addToLog(key, String(value));
}

void debug::addToLog(String key, double value)
{
    addToLog(key, String(value));
}

void debug::addToLog(String key, bool value)
{
    addToLog(key, String(value ? "true" : "false"));
}

void debug::addToLog(String key, long value)
{
    addToLog(key, String(value));
}

void debug::addToLog(String key, unsigned int value)
{
    addToLog(key, String(value));
}

void debug::quickLog(const String& message)
{
    clearBufferLog();
    addToLog("message", message);
    commitLog("System-Message", Severity::INFO);
}

void debug::enableRemoteLogging()
{
    remoteOverrideEnabled = true;
}

void debug::disableRemoteLogging()
{
    remoteOverrideEnabled = false;
}

void debug::setRemoteLoggingOverride(bool enabled)
{
    remoteOverrideEnabled = enabled;
}

bool debug::remoteLoggingOverrideEnabled()
{
    return remoteOverrideEnabled;
}

void debug::commitLog(String event, Severity severity)
{
    String jsonString = String(logBuffer.dump().c_str());

    if (isHigherSeverity(severity, config.printThreshold))
    {
        Serial.println(event);
        Serial.println(jsonString);
    }

    if (remoteOverrideEnabled && isHigherSeverity(severity, config.remoteLogThreshold))
    {
        Log newLog;
        jsonString.toCharArray(newLog.message, LOG_MESSAGE_MAX_LEN);
        newLog.severity = severity;
        event.toCharArray(newLog.event, LOG_EVENT_MAX_LEN);
        newLog.loggedAt = timing::unixNow();

        if (!trySendRemoteLog(newLog))
        {
            toRemoteLog.add(newLog);
        }
    }

    logBuffer.clear();
}

void debug::flushRemoteLogs()
{
    Log log;
    while (toRemoteLog.take(log))
    {
        if (!trySendRemoteLog(log))
        {
            toRemoteLog.add(log);
            break;
        }
    }
}

bool debug::hasRemoteLog()
{
    return !toRemoteLog.empty();
}

int debug::remoteLogCount()
{
    return toRemoteLog.size();
}

bool debug::takeRemoteLog(Log& output)
{
    return toRemoteLog.take(output);
}

const SleepSafeQueue<debug::Log, debug::DEBUG_MESSAGE_QUEUE>& debug::remoteLogs()
{
    return toRemoteLog;
}

const char* debug::severityToString(Severity severity)
{
    switch (severity)
    {
    case Severity::INFO:
        return "INFO";
    case Severity::WARN:
        return "WARN";
    case Severity::CRITICAL:
        return "CRITICAL";
    case Severity::STARTUP:
        return "STARTUP";
    default:
        return "UNKNOWN";
    }
}

bool debug::isHigherSeverity(Severity s1, Severity s2)
{
    Severity normalizedS1 = (s1 == Severity::STARTUP) ? Severity::CRITICAL : s1;
    Severity normalizedS2 = (s2 == Severity::STARTUP) ? Severity::CRITICAL : s2;

    return normalizedS1 >= normalizedS2;
}
