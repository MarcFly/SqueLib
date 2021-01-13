#ifdef TIMER_SOLO
    #include "fly_timer.h"
#else
    #include "fly_lib.h"
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// READABILITY TYPES / MACROS //////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <chrono>
typedef std::chrono::high_resolution_clock::duration high_res_clock;
#define TIME_NOW std::chrono::high_resolution_clock::now().time_since_epoch()
#define CAST_MILLI(now) (double)std::chrono::duration_cast<std::chrono::milliseconds>(now).count()
#define CAST_MICRO(now) (double)std::chrono::duration_cast<std::chrono::microseconds>(now).count()
#define CAST_NANOS(now) (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FLY_Timer::FLY_Timer() : is_stopped(false), is_active(true)
{
    high_res_clock now = std::chrono::high_resolution_clock::now().time_since_epoch();
    stop_at_ms = start_at_ms = CAST_MILLI(now);
    stop_at_us = start_at_us = CAST_MICRO(now);
    stop_at_ns = start_at_ns = CAST_NANOS(now);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLY_Timer::Start()
{
    is_active = true;
    is_stopped = false;
    high_res_clock now = TIME_NOW;
    stop_at_ms = start_at_ms = CAST_MILLI(now);
    stop_at_us = start_at_us = CAST_MICRO(now);
    stop_at_ns = start_at_ns = CAST_NANOS(now);
}

void FLY_Timer::Stop()
{
    high_res_clock now = TIME_NOW;
    stop_at_ms = CAST_MILLI(now);
    stop_at_us = CAST_MICRO(now);
    stop_at_ns = CAST_NANOS(now);
    is_stopped = true;
}

void FLY_Timer::Kill()
{
    is_active = false;
    stop_at_ms = 0;
    stop_at_us = 0;
    stop_at_ns = 0;
}

bool FLY_Timer::IsStopped()
{
    return is_stopped;
}

bool FLY_Timer::IsActive()
{
    return is_active;
}

double FLY_Timer::ReadMilliSec()
{
    if (is_stopped || !is_active)
        return stop_at_ms - start_at_ms;
    double now = CAST_MILLI(TIME_NOW);
    return now - start_at_ms;
}

double FLY_Timer::ReadMicroSec()
{
    if (is_stopped || !is_active)
        return stop_at_us - start_at_us;
    double now = CAST_MICRO(TIME_NOW);
    return now - start_at_us;
}

double FLY_Timer::ReadNanoSec()
{
    if (is_stopped || !is_active)
        return stop_at_ns - start_at_ns;
    double now = CAST_NANOS(TIME_NOW);
    return now - start_at_us;
}