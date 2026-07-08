#include <HTTPClient.h>
#include <Arduino.h>
#include "debug.h"
#include "networking/wifi_handler.h"

void WiFiHandler::setCredentials(const String &name, const String &password)
{
    wifiName = name;
    wifiPassword = password;
}

bool WiFiHandler::connect()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifiName, wifiPassword);


    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++)
    {

        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        delay(retryTime);
    }

    failedAttempts++;
    return false;
}

bool WiFiHandler::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool WiFiHandler::ensureConnection()
{
    if (isConnected() == false)
        return connect();
    else
        return true;
}

void WiFiHandler::disconnect()
{
    WiFi.disconnect(true); // disconnect and erase current connection from RAM
    WiFi.mode(WIFI_OFF);
}
