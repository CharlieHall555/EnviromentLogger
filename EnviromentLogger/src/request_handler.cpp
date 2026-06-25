#include "request_handler.h"
#include <HTTPClient.h>
#include "logger.h"

requestHandler::httpResponse requestHandler::postRequest(const postRequestParams& params)
{

    HTTPClient http;

    http.begin(params.URL);
    http.addHeader("Content-Type", "application/json");

    if (params.token != ""){
        http.addHeader("Authorization", params.token);
    }

    LOG("Sending:");

    int statusCode = http.POST(params.body);

    LOG("Status code: ");
    LOG(statusCode);

    String response = http.getString();
    LOG("Response:");
    LOG(response);

    http.end();

    return requestHandler::httpResponse{statusCode, response};
}

requestHandler::httpResponse requestHandler::getRequest(const getRequestParams &params)
{
    HTTPClient http;

    http.begin(params.URL);
    http.addHeader("Content-Type", "application/json");

    if (params.token != ""){
        http.addHeader("Authorization", params.token);
    }

    LOG("Sending:");

    int statusCode = http.GET();

    LOG("Status code: ");
    LOG(statusCode);

    String response = http.getString();
    LOG("Response:");
    LOG(response);

    http.end();
    return requestHandler::httpResponse{statusCode, response};
}
