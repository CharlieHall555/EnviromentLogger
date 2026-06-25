#include <HTTPClient.h>
#include <Arduino.h>
#include "logger.h"
#include "wifi_handler.h"

String wifiName;
String wifiPassword;

const int TIMEOUT_MS = 5000;

void wifi::setCredentials(String _name , String _password){
    wifiName = _name;
    wifiPassword = _password;
}

bool wifi::connect() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiName, wifiPassword);

    LOG("Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        LOG(".");
    }

    LOG("Connected!");
    LOG("ESP32 IP: ");
    LOG(WiFi.localIP());
    return true;
}

bool wifi::isConnected(){
    return WiFi.status() == WL_CONNECTED;
}

bool wifi::ensureConnection(){
    if (isConnected() == false) return connect();
    else return true;
}