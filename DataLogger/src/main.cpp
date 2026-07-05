#include <Arduino.h>
#include "secrets.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <queue>
#include "logger.h"
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
const int LOGGING_INTERVAL = 60 * 10; // seconds

#ifdef DEBUG
#define SLEEP(x) delay(x * 1000);
#else
#define SLEEP(x) esp_deep_sleep(x * 1000000);
#endif

void addReadingToRetryQueue(CombinedReading reading)
{

    LOG("Adding reading to retry queue.");

    retryQueue.add(reading);
    while (retryQueue.size() > MAX_RETRY_QUEUE_SIZE)
    {
        retryQueue.take();
    }
}

int sendReading(CombinedReading reading)
{
    String jsonBody = "{";
    jsonBody += "\"temperature\":" + String(reading.temp, 2) + ",";
    jsonBody += "\"humidity\":" + String(reading.hum, 2) + ",";
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
    Serial.begin(115200);
    Wire.begin(21, 22); // SDA, SCL
    Wire.setClock(100000);
    wifi::setCredentials(WIFI_NAME, WIFI_PASSWORD);
    tempSensor.startSensor();
    pmSensor.startSensor();

    while (wifi::isConnected() == false)
    {
        bool success = wifi::connect();
        if (success)
            continue;
        else
            delay(5000);
    }
    LOG("Connected to Wifi...");
    timing::setupBootTime();
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

void loop()
{

    while (pmSensor.isReady() == false)
        delay(100);
    while (tempSensor.isReady() == false)
        delay(100);

    bool hasWifi = wifi::ensureConnection();
    std::optional<TempReading> nextTempReading = tempSensor.takeReading();
    std::optional<PMReading> nextPMReading = pmSensor.takeReading();

    if (hasWifi && nextTempReading.has_value() && nextPMReading.has_value())
    {
        CombinedReading nextReading = constructCombined(nextTempReading.value(), nextPMReading.value());
        dumpReading(nextReading);
        int responseCode = sendReading(nextReading);
        bool retry = shouldRetry(responseCode);
        if (retry)
        {
            addReadingToRetryQueue(nextReading);
        }
    }

    if (retryQueue.size() > 0)
        flushQueue();

    wifi::disconnect();
    LOG("Entering deep sleep...");
    SLEEP(LOGGING_INTERVAL);
}