#include "pm_logger.h"
#include "logger.h"
#include <optional>

// PMS5003/7003 frame: 0x42 0x4D + 30 bytes (frame length word + 13 data words + checksum word)
static const size_t PMS_FRAME_LEN = 32;
static const uint8_t PMS_START1 = 0x42;
static const uint8_t PMS_START2 = 0x4D;

bool readingSanityCheck(PMReading reading)
{
    if (reading._1 < 0 || reading._1 > 1000)   return false;
    if (reading._2_5 < 0 || reading._2_5 > 1000) return false;
    if (reading._10 < 0 || reading._10 > 1000)  return false;
    if (reading._2_5 < reading._1)              return false;
    if (reading._10 < reading._2_5)             return false;
    return true;
}

PMSensor::PMSensor() : pmsSerial(2)
{
}

bool PMSensor::sensorPresent()
{
    // Flush stale bytes then wait briefly for fresh data
    while (pmsSerial.available()) pmsSerial.read();
    delay(200);
    LOG(pmsSerial.available() > 0);
    return pmsSerial.available() > 0;
}

void PMSensor::connectToSensor()
{
    pmsSerial.begin(9600, SERIAL_8N1, 16, 17);
    pmsSerial.setTimeout(1500);
}

bool PMSensor::ensureReady()
{
    if (sensorPresent())
    {
        return true;
    }
    else
    {
        LOG("Connection to PM sensor lost, reconnecting...");
        return PMSensor::startSensor();
    }
}

bool PMSensor::isReady()
{
    return sensorPresent();
}

bool PMSensor::startSensor()
{
    connectToSensor();

    for (int attempt = 1; attempt <= 10; attempt++)
    {
        LOG("Starting PM sensor, attempt ");
        LOG(attempt);

        if (sensorPresent())
        {
            LOG("PM sensor ready!");
            return true;
        }
        delay(500);
    }
    return false;
}

std::optional<PMReading> PMSensor::takeReading()
{
    ensureReady();

    // Synchronise to start-of-frame
    uint8_t buf[PMS_FRAME_LEN];
    unsigned long deadline = millis() + 2000;

    while (millis() < deadline)
    {
        if (pmsSerial.read() == PMS_START1)
        {
            if (pmsSerial.peek() == PMS_START2)
            {
                pmsSerial.read(); // consume START2
                // Read the remaining 30 bytes
                size_t got = pmsSerial.readBytes(buf + 2, PMS_FRAME_LEN - 2);
                if (got < PMS_FRAME_LEN - 2) break;

                buf[0] = PMS_START1;
                buf[1] = PMS_START2;

                // Verify checksum (sum of bytes 0..29)
                uint16_t sum = 0;
                for (size_t i = 0; i < PMS_FRAME_LEN - 2; i++) sum += buf[i];
                uint16_t checksum = ((uint16_t)buf[30] << 8) | buf[31];
                if (sum != checksum) break;

                // Bytes 4-5: PM1.0, 6-7: PM2.5, 8-9: PM10 (standard, μg/m³)
                PMReading reading;
                reading._1   = ((int)buf[4] << 8) | buf[5];
                reading._2_5 = ((int)buf[6] << 8) | buf[7];
                reading._10  = ((int)buf[8] << 8) | buf[9];

                if (readingSanityCheck(reading))
                    return reading;
                else
                    return std::nullopt;
            }
        }
    }

    return std::nullopt;
}