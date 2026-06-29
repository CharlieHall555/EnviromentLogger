#pragma once
#include <Arduino.h>

namespace requestHandler{

    struct postRequestParams{
        String URL;
        String body = "";
        String token = "";

    };

    struct getRequestParams{
        String URL;
        String token = "";
    };

    struct httpResponse{
        int code;
        String raw;
    };

    httpResponse postRequest(const postRequestParams& params);
    httpResponse getRequest(const getRequestParams& params);

}
