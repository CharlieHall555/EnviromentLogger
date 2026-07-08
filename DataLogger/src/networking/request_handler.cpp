#include "networking/request_handler.h"
#include <HTTPClient.h>
#include "debug.h"
#include "secrets.h"

bool requestHandler::responseSuccess(int code) {
    if (code >= 200 && code <= 299) return true;
    else return false;
}

requestHandler::httpResponse requestHandler::postRequest(const postRequestParams& params)
{
    HTTPClient http;

    http.begin(params.URL);
    http.addHeader("Content-Type", "application/json");

    if (params.token != ""){
        http.addHeader("Authorization", params.token);
    }

    bool previousRemoteOverride = debug::remoteLoggingOverrideEnabled();
    debug::setRemoteLoggingOverride(false);
    debug::addToLog("message" , "Sending post request to " + params.URL);
    debug::addToLog("request-body", params.body);
    debug::commitLog("Sending-Post-Request" , debug::Severity::INFO);

    int statusCode = http.POST(params.body);
    String response = http.getString();
    http.end();

    debug::Severity severity;

    if (requestHandler::responseSuccess(statusCode)) severity = debug::Severity::INFO;
    else severity = debug::Severity::WARN;

    debug::addToLog("response" , response);
    debug::addToLog("status-code" , statusCode);
    debug::commitLog("Post-Request-Response" , severity);
    debug::setRemoteLoggingOverride(previousRemoteOverride);

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

    bool previousRemoteOverride = debug::remoteLoggingOverrideEnabled();
    debug::setRemoteLoggingOverride(false);
    debug::addToLog("message" , "Sending get request to " + params.URL);
    debug::commitLog("Sending-Get-Request" , debug::Severity::INFO);

    int statusCode = http.GET();
    String response = http.getString();

    debug::addToLog("response" , response);
    debug::addToLog("status-code" , statusCode);
    debug::commitLog("Get-Request-Response" , debug::Severity::INFO);
    debug::setRemoteLoggingOverride(previousRemoteOverride);

    http.end();
    return requestHandler::httpResponse{statusCode, response};
}