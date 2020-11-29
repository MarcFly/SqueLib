#ifdef TIMER_SOLO
    #include "fly_timer.h"
#else
    #include "fly_lib.h"
#endif

#include <chrono>
#include <atomic>

typedef std::chrono::high_resolution_clock::duration high_res_clock;

FlyTimer::FlyTimer() : is_stopped(false)
{
    high_res_clock now = std::chrono::high_resolution_clock::now().time_since_epoch();
    stop_at_ms = start_at_ms = (uint16_t)std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    stop_at_us = start_at_us = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    stop_at_ns = start_at_ns = (uint32_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void FlyTimer::Start()
{
    high_res_clock now = std::chrono::high_resolution_clock::now().time_since_epoch();
    stop_at_ms = start_at_ms = (uint16_t)std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    stop_at_us = start_at_us = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    stop_at_ns = start_at_ns = (uint32_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
}

void FlyTimer::Stop()
{
    high_res_clock now = std::chrono::high_resolution_clock::now().time_since_epoch();
    stop_at_ms = (uint16_t)std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    stop_at_us = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(now).count();
    stop_at_ns = (uint32_t)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    is_stopped = true;
}

bool FlyTimer::IsStopped() const
{
    return is_stopped;
}

uint16_t FlyTimer::ReadMilliSec() const
{
    if (is_stopped)
        return stop_at_ms - start_at_ms;
    uint16_t now = (uint16_t)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    return now - start_at_ms;
}

uint32_t FlyTimer::ReadMicroSec() const
{
    if (is_stopped)
        return stop_at_us - start_at_us;
    uint32_t now = (uint32_t)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    return now - start_at_us;
}

uint32_t FlyTimer::ReadNanoSec() const
{
    if (is_stopped)
        return stop_at_ns - start_at_ns;
    uint32_t now = (uint32_t)std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    return now - start_at_us;
}