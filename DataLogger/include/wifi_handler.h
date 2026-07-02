#pragma once
#include <Arduino.h>

namespace wifi {
void setCredentials(String _name, String _password);
bool connect();
bool isConnected();
bool ensureConnection();
void disconnect();
};