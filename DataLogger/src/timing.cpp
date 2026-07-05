#include <Arduino.h>
#include <timing.h>
#include <request_handler.h>
#include <logger.hpp>
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

void timing::setupBootTime(){

    bool success = false;

    while (!success){
        requestHandler::httpResponse response = requestHandler::getRequest(
            {"https://aisenseapi.com/services/v1/timestamp"
        });

        if (response.code == 200){
            json data = json::parse(response.raw);
            int value = data.at("timestamp").get<int>();
            approxBootTime = value - unixNow();
            LOG("Success setting up approxBootTime: " + String(approxBootTime));
            return;
        }else{
            delay(5000); 
        }
    }

}