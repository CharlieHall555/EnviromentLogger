#pragma once
#include <Arduino.h>

class WiFiHandler{
    public:
        void setCredentials(const String &name, const String &password);
        bool connect();
        bool isConnected();
        bool ensureConnection();
        void disconnect();

    private:
        String wifiName;
        String wifiPassword;
        int failedAttempts = 0;
        int retryTime = 250;
        
        static constexpr int MAX_RETRIES = 5;
};