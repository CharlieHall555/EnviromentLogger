#include <Arduino.h>
#include <timing.h>
#include "networking/request_handler.h"
#include <json.hpp>

using json = nlohmann::json;

int approxBootTime = 0;

int timing::unixNow(){
    int secondsAlive = floor(millis() / 1000);
    return approxBootTime + secondsAlive;
};

int timing::getApproxBootTime(){
    return approxBootTime;
}

bool timing::setupBootTime(){
    bool success = false;

    requestHandler::httpResponse response = requestHandler::getRequest(
        {"https://aisenseapi.com/services/v1/timestamp"});

    if (response.code == 200){
        json data = json::parse(response.raw);
        int value = data.at("timestamp").get<int>();
        approxBootTime = value - unixNow();
        return true;
    }
    return false;
}