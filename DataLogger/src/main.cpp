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

TempSensor tempSensor;
const int MAX_RETRY_QUEUE_SIZE = 10;
RTC_DATA_ATTR ReadingQueue<MAX_RETRY_QUEUE_SIZE> retryQueue;
const int LOGGING_INTERVAL = 60 * 10; //seconds

void addReadingToRetryQueue(TempReading reading){

    LOG("Adding reading to retry queue.");

    retryQueue.add(reading);
    while (retryQueue.size() > MAX_RETRY_QUEUE_SIZE)
    {
        retryQueue.take();
    }
}   

bool sendReading(TempReading reading)
{
    String jsonBody = "{";
    jsonBody += "\"temperature\":" + String(reading.temp, 2) + ",";
    jsonBody += "\"humidity\":" + String(reading.hum, 2) + ",";
    jsonBody += "\"measured_at\":" + String(reading.time, 10);
    jsonBody += "}";

    LOG(jsonBody);

    requestHandler::httpResponse response = requestHandler::postRequest({API_URL, jsonBody, API_TOKEN});

    if (response.code >= 200 && response.code < 300) return true;
    else return false;
}

void flushQueue(){
    TempReading current;
    
    while (retryQueue.take(current)){
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

void loop()
{

    bool hasWifi = wifi::ensureConnection();
    std::optional<TempReading> newReading = tempSensor.takeReading();

    if (hasWifi && newReading.has_value())
    {
        bool successSend = sendReading(newReading.value());
        if (successSend == false){
            addReadingToRetryQueue(newReading.value());
        }
    }

    if (retryQueue.size() > 0) 
        flushQueue();

    wifi::disconnect();
    LOG("Entering deep sleep...");
    esp_deep_sleep(LOGGING_INTERVAL * 1000000);

}