#pragma once

namespace timing
{

    /// @brief Attempts to take an approximate reading of device unix boot time, yields until successful.
    bool setupBootTime();

    /// @brief Returns an approximate reading of device unix boot time.
    int getApproxBootTime();

    /// @brief Returns the devices approximation of unix time, Not guaranteed to be accurated or synced but good enough for approximate time keeping.
    /// @return int aproxUnixTime
    int unixNow();

};