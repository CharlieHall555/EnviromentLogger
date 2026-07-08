#include "sensors/temp_sensor.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <optional>
#include "debug.h"

const uint8_t address = 0x76;
const int MAX_RETRIES = 3;

bool TempSensor::readingSanityCheck(const TempReading& reading)
{
    if (isnan(reading.temp) || isnan(reading.hum))
        return false;
    else if (reading.temp < -20 || reading.temp > 80)
        return false;
    else if (reading.hum < 0 || reading.hum > 100)
        return false;
    else
        return true;
}

bool bmePresent()
{
    Wire.beginTransmission(0x76);
    return Wire.endTransmission() == 0;
}

bool TempSensor::sensorPresent()
{
    Wire.beginTransmission(0x76);
    return Wire.endTransmission() == 0 && takeReading().has_value();
}

bool TempSensor::ensureReady()
{
    if (sensorPresent())
    {
        return true;
    }
    else
    {
        return TempSensor::startSensor();
    }
}

TempSensor::TempSensor()
{
}

bool TempSensor::setup()
{
    Wire.begin();
    return TempSensor::startSensor();
}


bool TempSensor::startSensor()
{
    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++)
    {

        if (sensor.begin(address, &Wire))
        {
            return true;
        }

        delay(2500);
    }
    return false;
}

std::optional<TempReading> TempSensor::takeReading()
{

    float temp = sensor.readTemperature();
    float hum = sensor.readHumidity();

    TempReading output;
    output.temp = temp;
    output.hum = hum;

    if (TempSensor::readingSanityCheck(output))
        return output;
    else
        return std::nullopt;
}
