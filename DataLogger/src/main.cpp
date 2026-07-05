#include <Arduino.h>
#include "secrets.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <queue>
#include "logger.hpp"
#include "wifi_handler.h"
#include "request_handler.h"
#include <readingQueue.hpp>
#include "timing.h"
#include <Wire.h>
#include "temp_logger.h"
#include <optional>
#include <pm_logger.h>
#include "combined_reading.h"

TempSensor tempSensor;
PMSensor pmSensor;
const int MAX_RETRY_QUEUE_SIZE = 10;
RTC_DATA_ATTR ReadingQueue<MAX_RETRY_QUEUE_SIZE> retryQueue;
const int LOGGING_INTERVAL = 10; // seconds

#ifdef DEBUG
#define SLEEP(seconds) delay(seconds * 1000);
#else
#define SLEEP(seconds) esp_deep_sleep(seconds * 1000000);
#endif

RTC_DATA_ATTR struct SystemState
{
    bool wifiOk;
    bool pmSensorOk;
    bool tempSensorOk;
    int approxBootTime;
} systemState;

const char* okFail(bool value)
{
    return value ? "OK" : "FAIL";
}

void dumpSystemState()
{
    LOG("===== SYSTEM STATE =====");
    LOG("WiFi: ");
    LOG(okFail(systemState.wifiOk));
    LOG("PM Sensor: ");
    LOG(okFail(systemState.pmSensorOk));
    LOG("Temp Sensor: ");
    LOG(okFail(systemState.tempSensorOk));
}

void addReadingToRetryQueue(CombinedReading reading)
{

    LOG("Adding reading to retry queue.");

    while (retryQueue.size() >= MAX_RETRY_QUEUE_SIZE)
    {
        retryQueue.take();
    }
    retryQueue.add(reading);
}

int sendReading(CombinedReading reading)
{
    String jsonBody = "{";
    jsonBody += "\"temperature\":" + String(reading.temp, 2) + ",";
    jsonBody += "\"humidity\":" + String(reading.hum, 2) + ",";
    jsonBody += "\"pm1_0\":" + String(reading._1) + ",";
    jsonBody += "\"pm2_5\":" + String(reading._2_5) + ",";
    jsonBody += "\"pm10\":" + String(reading._10) + ",";
    jsonBody += "\"measured_at\":" + String(reading.time, 10);
    jsonBody += "}";

    LOG(jsonBody);

    requestHandler::httpResponse response = requestHandler::postRequest({API_URL, jsonBody, API_TOKEN});

    return response.code;
}

void flushQueue()
{
    CombinedReading current;

    while (retryQueue.take(current))
    {
        sendReading(current);
        delay(100);
    }
}

bool connected = false;

void setup()
{
    LOG("--STARTING DEVICE--");
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA, SCL
    Wire.setClock(100000);

    wifi::setCredentials(WIFI_NAME, WIFI_PASSWORD);
    systemState.tempSensorOk = tempSensor.startSensor();
    systemState.pmSensorOk = pmSensor.startSensor();
    systemState.wifiOk = wifi::connect();
    dumpSystemState();

    if (systemState.wifiOk){
        timing::setupBootTime();
        systemState.approxBootTime = timing::getApproxBootTime();       
    }else{
        LOG("Failed start up conditions, Sleeping...");
        SLEEP(10 * 60);
    }
    
}

bool shouldRetry(int returnCode)
{
    if (returnCode == 422) // body is deformed there is no real point of retrying.
        return false;
    else if (returnCode >= 200 && returnCode <= 299)
        return false;
    else
        return true;
}

void handleReading()
{
    std::optional<TempReading> nextTempReading = tempSensor.takeReading();
    std::optional<PMReading> nextPMReading = pmSensor.takeReading();

    if (nextTempReading.has_value() && nextPMReading.has_value())
    {
        CombinedReading nextReading = constructCombined(nextTempReading.value(), nextPMReading.value());
        dumpReading(nextReading);
        bool retry = false;

        if (systemState.wifiOk)
        {
            int responseCode = sendReading(nextReading);
            retry = shouldRetry(responseCode);
        }
        else
            retry = true;

        if (retry)
            addReadingToRetryQueue(nextReading);
    }
}

void loop()
{
    systemState.pmSensorOk = pmSensor.ensureReady();
    systemState.tempSensorOk = tempSensor.ensureReady();
    systemState.wifiOk = wifi::ensureConnection();

    if (systemState.pmSensorOk && systemState.tempSensorOk)
        handleReading();

    if (retryQueue.size() > 0 && systemState.wifiOk)
        flushQueue();

    wifi::disconnect();
    LOG("Entering deep sleep...");
    SLEEP(LOGGING_INTERVAL);
}