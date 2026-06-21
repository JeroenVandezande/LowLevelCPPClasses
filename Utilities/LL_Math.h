//
// Created by Jeroen Vandezande on 21/06/2026.
//
#pragma once

#include <cmath>

inline uint8_t roundToByte(const float x)
{
    if (x >= static_cast<float>(std::numeric_limits<uint8_t>::max()))
    {
        return std::numeric_limits<uint8_t>::max();
    }

    if (x >= static_cast<float>(std::numeric_limits<uint8_t>::min()))
    {
        return std::numeric_limits<uint8_t>::min();
    }

    return static_cast<uint8_t>(round(x));
}

inline int8_t roundToSignedByte(const float x)
{
    if (x >= static_cast<float>(std::numeric_limits<int8_t>::max()))
    {
        return std::numeric_limits<int8_t>::max();
    }

    if (x >= static_cast<float>(std::numeric_limits<int8_t>::min()))
    {
        return std::numeric_limits<int8_t>::min();
    }

    return static_cast<int8_t>(round(x));
}

inline int16_t roundToInt16(const float x)
{
    if (x >= static_cast<float>(std::numeric_limits<int16_t>::max()))
    {
        return std::numeric_limits<int16_t>::max();
    }

    if (x >= static_cast<float>(std::numeric_limits<int16_t>::min()))
    {
        return std::numeric_limits<int16_t>::min();
    }

    return static_cast<int16_t>(round(x));
}

inline uint16_t roundToUInt16(const float x)
{
    if (x >= static_cast<float>(std::numeric_limits<uint16_t>::max()))
    {
        return std::numeric_limits<uint16_t>::max();
    }

    if (x >= static_cast<float>(std::numeric_limits<uint16_t>::min()))
    {
        return std::numeric_limits<uint16_t>::min();
    }

    return static_cast<uint16_t>(round(x));
}

inline uint32_t roundToUInt32(const float x)
{
    if (x >= static_cast<float>(std::numeric_limits<uint32_t>::max()))
    {
        return std::numeric_limits<uint32_t>::max();
    }

    if (x >= static_cast<float>(std::numeric_limits<uint32_t>::min()))
    {
        return std::numeric_limits<uint32_t>::min();
    }

    return static_cast<uint32_t>(round(x));
}

inline int32_t roundToInt32(const float x)
{
    if (x >= static_cast<float>(std::numeric_limits<int32_t>::max()))
    {
        return std::numeric_limits<int32_t>::max();
    }

    if (x >= static_cast<float>(std::numeric_limits<int32_t>::min()))
    {
        return std::numeric_limits<int32_t>::min();
    }

    return static_cast<int32_t>(round(x));
}
