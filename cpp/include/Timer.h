#pragma once

#include <chrono>

/*
    Timer
    Utility class to calculate elapsed time between events.
*/

class Timer
{
public:
    Timer() : m_startTime(std::chrono::high_resolution_clock::now()) {}

    // Returns the number of seconds elapsed since the last tick
    double tick()
    {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = now - m_startTime;
        m_startTime = now;
        return elapsed_seconds.count();
    }

private:
    std::chrono::high_resolution_clock::time_point m_startTime;
};
  