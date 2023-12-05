/**
 * @file Debug.h
 * @author Zhao Yuanfang (zhaoyuanfang@dameng.com)
 * @brief
 * @date 2023-12-03
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "Common.h"

#include <chrono>
#include <ratio>

namespace dm {

#if !defined(NDEBUG)
#define assert_r(cond, ret) do {                            \
    if (!(cond))                                            \
    {                                                       \
        std::stringstream ss;                               \
        ss << "assert on \"" << #cond                       \
                << "\" at " << __FILE__ << ":" << __LINE__  \
                << " failed.";                              \
        print_assert(ss.str());                             \
        return (ret);                                       \
    }                                                       \
} while(0);
#else
#define assert_r(cond, ret)
#endif

#if !defined(NDEBUG)
#define assert_nr(cond) do {                                \
    if (!(cond))                                            \
    {                                                       \
        std::stringstream ss;                               \
        ss << "assert on \"" << #cond                       \
                << "\" at " << __FILE__ << ":" << __LINE__  \
                << " failed.";                              \
        print_assert(ss.str());                             \
        return;                                             \
    }                                                       \
} while(0);
#else
#define assert_nr(cond)
#endif

inline void print_assert(const String &msg)
{
    std::cerr << msg << std::endl;
}

class ScopeCounter
{
    i64 &mIntervalMilli;
    std::chrono::time_point<std::chrono::steady_clock>  mStart;

public:
    ScopeCounter(i64 &interval_milli)
    :
    mIntervalMilli(interval_milli),
    mStart(std::chrono::steady_clock::now())
    {

    }

    ~ScopeCounter()
    {
        auto span = std::chrono::steady_clock::now() - mStart;  // ns
        mIntervalMilli = span.count() / 1'000'000;   // ms
    }
};

}   // end of namespace dm
