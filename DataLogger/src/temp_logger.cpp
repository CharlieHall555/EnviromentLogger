#include "temp_logger.h"
#include "logger.h"
#include <Wire.h>
#include <Adafruit_BME280.h>
#include "timing.h"
#include <optional>

const uint8_t address = 0x76;

bool readingSanityCheck(TempReading reading)
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
    return Wire.endTransmission() == 0;
}

bool TempSensor::ensureReady()
{
    if (sensorPresent())
    {
        return true;
    }
    else
    {
        LOG("Connection to sensor lost, Reconnecting...");
        return TempSensor::startSensor();
    }
}

TempSensor::TempSensor()
{
}

bool TempSensor::isReady()
{
    return sensorPresent();
}

bool TempSensor::startSensor()
{
    for (int attempt = 1; attempt <= 10; attempt++)
    {
        LOG("Starting BME280 attempt ");
        LOG(attempt);

        if (sensor.begin(address, &Wire))
        {
            LOG("BME280 started!");
            return true;
        }

        delay(500);
    }
    return false;
}

std::optional<TempReading> TempSensor::takeReading()
{

    ensureReady();

    float temp = sensor.readTemperature();
    float hum = sensor.readHumidity();

    TempReading output;
    output.temp = temp;
    output.hum = hum;
    output.time = timing::unixNow();

    if (readingSanityCheck(output))
        return output;
    else
        return std::nullopt;
}
