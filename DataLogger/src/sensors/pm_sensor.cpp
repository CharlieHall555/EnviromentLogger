#include "sensors/pm_sensor.h"
#include "debug.h"
#include "Arduino.h"
#include <optional>
#include "domain/combined_reading.h"

static const size_t PMS_FRAME_LEN = 32;
static const uint8_t PMS_START1 = 0x42;
static const uint8_t PMS_START2 = 0x4D;
static const int SET_PIN = 12;

PMSensor::PMSensor() : pmsSerial(2){}

bool PMSensor::readingSanityCheck(const PMReading &reading)
{
    if (reading._1 < 0 || reading._2_5 < 0 || reading._10 < 0)
        return false;
    if (reading._1 > 1000 || reading._2_5 > 1000 || reading._10 > 1000)
        return false;
    return true;
}

PMReadingQuality PMSensor::getReadingQuality(const PMReading& reading)
{
    constexpr int PM1_ELEVATED_THRESHOLD = 20;
    constexpr int PM2_5_ELEVATED_THRESHOLD = 25;
    constexpr int PM10_ELEVATED_THRESHOLD = 75;

    if (
        reading._1 > PM1_ELEVATED_THRESHOLD ||
        reading._2_5 > PM2_5_ELEVATED_THRESHOLD ||
        reading._10 > PM10_ELEVATED_THRESHOLD
    ) {
        return PMReadingQuality::ELEVATED;
    }

    return PMReadingQuality::NORMAL;
}

PMReadingQuality PMSensor::getReadingQuality(const CombinedReading& reading)
{
    constexpr int PM1_ELEVATED_THRESHOLD   = 20;
    constexpr int PM2_5_ELEVATED_THRESHOLD = 25;
    constexpr int PM10_ELEVATED_THRESHOLD  = 75;

    if (
        reading._1 >= PM1_ELEVATED_THRESHOLD ||
        reading._2_5 >= PM2_5_ELEVATED_THRESHOLD ||
        reading._10  >= PM10_ELEVATED_THRESHOLD
    ) {
        return PMReadingQuality::ELEVATED;
    }

    return PMReadingQuality::NORMAL;
}

bool PMSensor::setup()
{
    pinMode(SET_PIN, OUTPUT);
    digitalWrite(SET_PIN, HIGH);
    wake();
    startTime = floor(millis() / 1000);
    pmsSerial.begin(9600, SERIAL_8N1, 16, 17);
    pmsSerial.setTimeout(1500);
    return true;
}

int PMSensor::timeActive()
{
    return floor(millis() / 1000) - startTime;
}

void PMSensor::wake() {
    gpio_hold_dis(GPIO_NUM_12);
    digitalWrite(SET_PIN, HIGH);
    startTime = millis() / 1000;
}

void PMSensor::sleep() {
    digitalWrite(SET_PIN, LOW);
    gpio_hold_en(GPIO_NUM_12);
    gpio_deep_sleep_hold_en();
}

bool PMSensor::isSendingData()
{
    uint32_t start = millis();

    while (millis() - start < 1000)
    {
        if (pmsSerial.available() >= 32)
        {
            if (pmsSerial.peek() == 0x42)
            {
                return true;
            }

            pmsSerial.read(); // discard bad byte
        }
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

std::optional<PMReading> PMSensor::takeAverageReading(int samples , int delayTime=1000)
{
    int pm1 = 0;
    int pm25 = 0;
    int pm10 = 0;
    int valid = 0;

    for (int i = 0; i < samples; i++) {
        auto reading = takeReading();

        if (reading.has_value()) {
            pm1 += reading->_1;
            pm25 += reading->_2_5;
            pm10 += reading->_10;
            valid++;
        }

        delay(delayTime);
    }

    if (valid == 0) {
        return std::nullopt;
    }

    return PMReading{
        pm1 / valid,
        pm25 / valid,
        pm10 / valid
    };
}

bool PMSensor::ensureReady()
{
    wake();

    // Give PMS sensor time to stabilise after waking.
    if (timeActive() < 30) {
        delay((30 - timeActive()) * 1000);
    }

    return isSendingData();
}   