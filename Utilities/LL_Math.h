//
// Created by Jeroen Vandezande on 21/06/2026.
//
#pragma once

#include <cstdint>
#include <cmath>
#include <limits>

namespace ll_math_detail
{
    enum class RoundCastMode
    {
        Round,
        Floor,
        Ceil,
    };

    template<typename T>
    inline T ClampValue(const long double x, const T min, const T max)
    {
        if (x >= static_cast<long double>(max))
        {
            return max;
        }

        if (x <= static_cast<long double>(min))
        {
            return min;
        }

        return static_cast<T>(x);
    }

    template<typename T>
    inline T CastToClamped(const long double x, const T min, const T max, const RoundCastMode mode)
    {
        switch (mode)
        {
            case RoundCastMode::Round:
                return ClampValue(std::round(x), min, max);
            case RoundCastMode::Floor:
                return ClampValue(std::floor(x), min, max);
            case RoundCastMode::Ceil:
                return ClampValue(std::ceil(x), min, max);
        }

        return ClampValue(std::round(x), min, max);
    }

    template<typename T, typename X>
    inline T CastToClamped(const X x, const T min, const T max, const RoundCastMode mode)
    {
        return CastToClamped(static_cast<long double>(x), min, max, mode);
    }
}

/**
 * Rounds a numeric value to the nearest integer and clamps it to the full range of the requested return type.
 *
 * @tparam T Integer return type to cast to.
 * @tparam X Numeric input type.
 * @param x Value to round and cast.
 * @return Rounded value clamped to std::numeric_limits<T>::min() and std::numeric_limits<T>::max().
 */
template<typename T, typename X>
inline T roundToClamped(const X x)
{
    return ll_math_detail::CastToClamped(x, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), ll_math_detail::RoundCastMode::Round);
}

/**
 * Rounds a numeric value to the nearest integer and clamps it to a caller-provided range.
 *
 * @tparam T Integer return type to cast to.
 * @tparam X Numeric input type.
 * @param x Value to round and cast.
 * @param min Minimum value returned after clamping.
 * @param max Maximum value returned after clamping.
 * @return Rounded value clamped to the provided range.
 */
template<typename T, typename X>
inline T roundToClamped(const X x, const T min, const T max)
{
    return ll_math_detail::CastToClamped(x, min, max, ll_math_detail::RoundCastMode::Round);
}

/**
 * Floors a numeric value and clamps it to the full range of the requested return type.
 *
 * @tparam T Integer return type to cast to.
 * @tparam X Numeric input type.
 * @param x Value to floor and cast.
 * @return Floored value clamped to std::numeric_limits<T>::min() and std::numeric_limits<T>::max().
 */
template<typename T, typename X>
inline T floorToClamped(const X x)
{
    return ll_math_detail::CastToClamped(x, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), ll_math_detail::RoundCastMode::Floor);
}

/**
 * Floors a numeric value and clamps it to a caller-provided range.
 *
 * @tparam T Integer return type to cast to.
 * @tparam X Numeric input type.
 * @param x Value to floor and cast.
 * @param min Minimum value returned after clamping.
 * @param max Maximum value returned after clamping.
 * @return Floored value clamped to the provided range.
 */
template<typename T, typename X>
inline T floorToClamped(const X x, const T min, const T max)
{
    return ll_math_detail::CastToClamped(x, min, max, ll_math_detail::RoundCastMode::Floor);
}

/**
 * Ceils a numeric value and clamps it to the full range of the requested return type.
 *
 * @tparam T Integer return type to cast to.
 * @tparam X Numeric input type.
 * @param x Value to ceil and cast.
 * @return Ceiled value clamped to std::numeric_limits<T>::min() and std::numeric_limits<T>::max().
 */
template<typename T, typename X>
inline T ceilToClamped(const X x)
{
    return ll_math_detail::CastToClamped(x, std::numeric_limits<T>::min(), std::numeric_limits<T>::max(), ll_math_detail::RoundCastMode::Ceil);
}

/**
 * Ceils a numeric value and clamps it to a caller-provided range.
 *
 * @tparam T Integer return type to cast to.
 * @tparam X Numeric input type.
 * @param x Value to ceil and cast.
 * @param min Minimum value returned after clamping.
 * @param max Maximum value returned after clamping.
 * @return Ceiled value clamped to the provided range.
 */
template<typename T, typename X>
inline T ceilToClamped(const X x, const T min, const T max)
{
    return ll_math_detail::CastToClamped(x, min, max, ll_math_detail::RoundCastMode::Ceil);
}

#define LL_MATH_DEFINE_CLAMPED_CASTS(TYPE, NAME)                                                                   \
    /**                                                                                                            \
     * Rounds a numeric value to the nearest integer and clamps it to the target return type range.               \
     *                                                                                                             \
     * @tparam X Numeric input type.                                                                               \
     * @param x Value to round and cast.                                                                           \
     * @return Rounded value clamped to the target return type range.                                             \
     */                                                                                                            \
    template<typename X>                                                                                           \
    inline TYPE roundTo##NAME##Clamped(const X x)                                                                  \
    {                                                                                                              \
        return roundToClamped<TYPE>(x);                                                                            \
    }                                                                                                              \
                                                                                                                   \
    /**                                                                                                            \
     * Rounds a numeric value to the nearest integer and clamps it to a caller-provided target return type range.  \
     *                                                                                                             \
     * @tparam X Numeric input type.                                                                               \
     * @param x Value to round and cast.                                                                           \
     * @param min Minimum value in the target return type after clamping.                                          \
     * @param max Maximum value in the target return type after clamping.                                          \
     * @return Rounded value clamped to the provided target return type range.                                     \
     */                                                                                                            \
    template<typename X>                                                                                           \
    inline TYPE roundTo##NAME##Clamped(const X x, const TYPE min, const TYPE max)                                  \
    {                                                                                                              \
        return roundToClamped<TYPE>(x, min, max);                                                                  \
    }                                                                                                              \
                                                                                                                   \
    /**                                                                                                            \
     * Floors a numeric value and clamps it to the target return type range.                                       \
     *                                                                                                             \
     * @tparam X Numeric input type.                                                                               \
     * @param x Value to floor and cast.                                                                           \
     * @return Floored value clamped to the target return type range.                                              \
     */                                                                                                            \
    template<typename X>                                                                                           \
    inline TYPE floorTo##NAME##Clamped(const X x)                                                                  \
    {                                                                                                              \
        return floorToClamped<TYPE>(x);                                                                            \
    }                                                                                                              \
                                                                                                                   \
    /**                                                                                                            \
     * Floors a numeric value and clamps it to a caller-provided target return type range.                         \
     *                                                                                                             \
     * @tparam X Numeric input type.                                                                               \
     * @param x Value to floor and cast.                                                                           \
     * @param min Minimum value in the target return type after clamping.                                          \
     * @param max Maximum value in the target return type after clamping.                                          \
     * @return Floored value clamped to the provided target return type range.                                     \
     */                                                                                                            \
    template<typename X>                                                                                           \
    inline TYPE floorTo##NAME##Clamped(const X x, const TYPE min, const TYPE max)                                  \
    {                                                                                                              \
        return floorToClamped<TYPE>(x, min, max);                                                                  \
    }                                                                                                              \
                                                                                                                   \
    /**                                                                                                            \
     * Ceils a numeric value and clamps it to the target return type range.                                        \
     *                                                                                                             \
     * @tparam X Numeric input type.                                                                               \
     * @param x Value to ceil and cast.                                                                            \
     * @return Ceiled value clamped to the target return type range.                                               \
     */                                                                                                            \
    template<typename X>                                                                                           \
    inline TYPE ceilTo##NAME##Clamped(const X x)                                                                   \
    {                                                                                                              \
        return ceilToClamped<TYPE>(x);                                                                             \
    }                                                                                                              \
                                                                                                                   \
    /**                                                                                                            \
     * Ceils a numeric value and clamps it to a caller-provided target return type range.                          \
     *                                                                                                             \
     * @tparam X Numeric input type.                                                                               \
     * @param x Value to ceil and cast.                                                                            \
     * @param min Minimum value in the target return type after clamping.                                          \
     * @param max Maximum value in the target return type after clamping.                                          \
     * @return Ceiled value clamped to the provided target return type range.                                      \
     */                                                                                                            \
    template<typename X>                                                                                           \
    inline TYPE ceilTo##NAME##Clamped(const X x, const TYPE min, const TYPE max)                                   \
    {                                                                                                              \
        return ceilToClamped<TYPE>(x, min, max);                                                                   \
    }

LL_MATH_DEFINE_CLAMPED_CASTS(int8_t, SignedByte)
LL_MATH_DEFINE_CLAMPED_CASTS(uint8_t, Byte)
LL_MATH_DEFINE_CLAMPED_CASTS(int8_t, Int8)
LL_MATH_DEFINE_CLAMPED_CASTS(uint8_t, UInt8)
LL_MATH_DEFINE_CLAMPED_CASTS(int16_t, Int16)
LL_MATH_DEFINE_CLAMPED_CASTS(uint16_t, UInt16)
LL_MATH_DEFINE_CLAMPED_CASTS(int32_t, Int32)
LL_MATH_DEFINE_CLAMPED_CASTS(uint32_t, UInt32)
LL_MATH_DEFINE_CLAMPED_CASTS(int64_t, Int64)
LL_MATH_DEFINE_CLAMPED_CASTS(uint64_t, UInt64)

#undef LL_MATH_DEFINE_CLAMPED_CASTS
