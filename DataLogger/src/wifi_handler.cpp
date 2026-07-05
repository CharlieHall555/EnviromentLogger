#include <HTTPClient.h>
#include <Arduino.h>
#include "logger.hpp"
#include "wifi_handler.h"

String wifiName;
String wifiPassword;

const int TIMEOUT_MS = 5000;
const int MAX_RETRIES = 5;

void wifi::setCredentials(String _name, String _password)
{
    wifiName = _name;
    wifiPassword = _password;
}

bool wifi::connect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiName, wifiPassword);

    LOG("Connecting to WiFi");

    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++)
    {

        if (WiFi.status() == WL_CONNECTED)
        {
            LOG("Connected!");
            LOG("ESP32 IP: ");
            LOG(WiFi.localIP());
            return true;
        }
        delay(2500);
        LOG(".");
    }

    return false;
}

bool wifi::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool wifi::ensureConnection()
{
    if (isConnected() == false)
        return connect();
    else
        return true;
}

void wifi::disconnect()
{
    WiFi.disconnect(true); // disconnect and erase current connection from RAM
    WiFi.mode(WIFI_OFF);
}
