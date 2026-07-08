#include "system.h"
#include "debug.h"
#include "networking/wifi_handler.h"
#include "networking/request_handler.h"
#include "json.hpp"
#include "secrets.h"
#include "timing.h"
#include "domain/combined_reading.h"
#include <optional>

using json = nlohmann::json;


/*
    String jsonBody = "{";
    jsonBody += "\"temperature\":" + String(reading.temp, 2) + ",";
    jsonBody += "\"humidity\":" + String(reading.hum, 2) + ",";
    jsonBody += "\"pm1_0\":" + String(reading._1) + ",";
    jsonBody += "\"pm2_5\":" + String(reading._2_5) + ",";
    jsonBody += "\"pm10\":" + String(reading._10) + ",";
    jsonBody += "\"measured_at\":" + String(reading.time, 10);
    jsonBody += "}"
*/

void LoggerSystem::pushNewReading(CombinedReading& reading)
{
    debug::quickLog("Sending reading to API...");
    String dataloggingEndPoint = String(BASE_API_URL) + String(DATA_LOG_ENDPOINT);

    requestHandler::postRequestParams requestParams;
    requestParams.URL = dataloggingEndPoint;
    requestParams.token = String(API_TOKEN);

    json requestBody;
    requestBody["temperature"] = reading.temp;
    requestBody["humidity"] = reading.hum;
    requestBody["pm1_0"] = reading._1;
    requestBody["pm2_5"] = reading._2_5;
    requestBody["pm10"] = reading._10;
    requestBody["measured_at"] = reading.time;

    requestParams.body = String(requestBody.dump().c_str());
    requestHandler::httpResponse response = requestHandler::postRequest(requestParams);
    if (!requestHandler::responseSuccess(response.code))
    {
        debug::quickLog("Reading upload failed, queued for retry");
        savedReadingQueue.add(reading);
        return;
    }

    debug::quickLog("Reading upload success");
}

void LoggerSystem::logSystemState()
{
    debug::clearBufferLog();
    debug::addToLog("initFlags.wifi", systemState.initFlags.wifi);
    debug::addToLog("initFlags.tempSensor", systemState.initFlags.tempSensor);
    debug::addToLog("initFlags.pmSensor", systemState.initFlags.pmSensor);
    debug::addToLog("initFlags.timing", systemState.initFlags.timing);
    debug::addToLog("startTime" , timing::getApproxBootTime());
    debug::commitLog("System-State", debug::Severity::STARTUP);
}

LoggerSystem::LoggerSystem()
{
}

bool LoggerSystem::takeReading(CombinedReading &reading)
{
    debug::quickLog("Taking sensor readings...");

    std::optional<TempReading> temp = tempSensor.takeReading();
    std::optional<PMReading> pm = pmSensor.takeReading();
    int time = timing::unixNow();
    
    if (tempSensor.sensorPresent() == false && pmSensor.isSendingData() == false) 
        return false;

    if (time && temp.has_value() && pm.has_value())
    {
        reading = constructCombined(temp.value(), pm.value(), time);
        debug::quickLog("Sensor readings captured");
        return true;
    }

    debug::quickLog("Sensor reading failed");
    return false;
}

void LoggerSystem::begin()
{
    startupRoutine();
    debug::quickLog("Waiting for pm sensor startup time...");
    while (pmSensor.timeActive() < 30)
        delay(1000);
    debug::quickLog("Waited for pm sensor, taking reading...");
    
    CombinedReading nextReading;
    bool readingSuccess = takeReading(nextReading);
    if (readingSuccess)
    {
        pushNewReading(nextReading);
    }
    else
    {
        debug::quickLog("Skipping upload because reading failed");
    }

    debug::quickLog("Took reading entering sleep");
    pmSensor.sleep();
    esp_deep_sleep(DEEP_SLEEP_TIME);
}

void LoggerSystem::startupRoutine()
{
    wifiHandler.setCredentials(WIFI_NAME, WIFI_PASSWORD);
    systemState.initFlags.wifi = wifiHandler.connect();
    systemState.initFlags.pmSensor = pmSensor.setup();
    systemState.initFlags.tempSensor = tempSensor.setup();
    systemState.initFlags.timing = timing::setupBootTime();
    logSystemState();
    debug::flushRemoteLogs();
    debug::quickLog("Startup routine complete");
}