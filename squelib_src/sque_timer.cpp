#ifdef TIMER_SOLO
    #include "sque_timer.h"
#else
    #include "squelib.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// READABILITY TYPES / MACROS //////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::chrono::high_resolution_clock::duration high_res_clock;
#define TIME_NOW std::chrono::high_resolution_clock::now().time_since_epoch()
#define CAST_MILLI(now) (double)std::chrono::duration_cast<std::chrono::milliseconds>(now).count()
#define CAST_MICRO(now) (double)std::chrono::duration_cast<std::chrono::microseconds>(now).count()
#define CAST_NANOS(now) (double)std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQUE_Timer::SQUE_Timer() : is_stopped(false), is_active(true)
{
    high_res_clock now = std::chrono::high_resolution_clock::now().time_since_epoch();
    stop_at_ms = start_at_ms = CAST_MILLI(now);
    stop_at_us = start_at_us = CAST_MICRO(now);
    stop_at_ns = start_at_ns = CAST_NANOS(now);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SQUE_Timer::Start()
{
    is_active = true;
    is_stopped = false;
    high_res_clock now = TIME_NOW;
    stop_at_ms = start_at_ms = CAST_MILLI(now);
    stop_at_us = start_at_us = CAST_MICRO(now);
    stop_at_ns = start_at_ns = CAST_NANOS(now);
}

void SQUE_Timer::Stop()
{
    high_res_clock now = TIME_NOW;
    stop_at_ms = CAST_MILLI(now);
    stop_at_us = CAST_MICRO(now);
    stop_at_ns = CAST_NANOS(now);
    is_stopped = true;
}

void SQUE_Timer::Kill()
{
    is_active = false;
    stop_at_ms = 0;
    stop_at_us = 0;
    stop_at_ns = 0;
}

bool SQUE_Timer::IsStopped() const
{
    return is_stopped;
}

bool SQUE_Timer::IsActive() const
{
    return is_active;
}

double SQUE_Timer::ReadMilliSec() const
{
    if (is_stopped || !is_active)
        return stop_at_ms - start_at_ms;
    double now = CAST_MILLI(TIME_NOW);
    return now - start_at_ms;
}

double SQUE_Timer::ReadMicroSec() const
{
    if (is_stopped || !is_active)
        return stop_at_us - start_at_us;
    double now = CAST_MICRO(TIME_NOW);
    return now - start_at_us;
}

double SQUE_Timer::ReadNanoSec() const
{
    if (is_stopped || !is_active)
        return stop_at_ns - start_at_ns;
    double now = CAST_NANOS(TIME_NOW);
    return now - start_at_us;
}