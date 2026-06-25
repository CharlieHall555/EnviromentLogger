#include <Arduino.h>
#include "secrets.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <queue>
#include "logger.h"
#include "wifi_handler.h"
#include "request_handler.h"

void saveReading(float temp , float hum){

}

void sendReading(float temperature, float humidity) {
    wifi::ensureConnection();
    String jsonBody = "{";
    jsonBody += "\"temperature\":" + String(temperature, 2) + ",";
    jsonBody += "\"humidity\":" + String(humidity, 2);
    jsonBody += "}";

    requestHandler::postRequest({
        API_URL , jsonBody , API_TOKEN
    });
}

void setup() {
    Serial.begin(115200);
    delay(5000);

    wifi::setCredentials(WIFI_NAME , WIFI_PASSWORD);
    wifi::connect();
}

void loop() {
   
    requestHandler::httpResponse response = requestHandler::getRequest(
        {"https://aisenseapi.com/services/v1/timestamp"
    });
    
    LOG(response.raw);


    delay(5000);
}