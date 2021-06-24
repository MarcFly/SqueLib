#ifndef _TEST_SQUE_BASICS_
#define _TEST_SQUE_BASICS_

#define TIMES 1000000
#define TIMES_S 1000

// Logging Performance
#include <squelib.h>

// Timer Performance
void TimerPerformance()
{
    SQUE_Timer t;
    SQUE_Timer t2;

    t.Start();
    for (int i = 0; i < TIMES; ++i)
        double test = t2.ReadMilliSec();
    t.Stop();
    SQUE_LOG(LT_INFO, "SQUE_Timer::ReadMilliSec(): %f ms", t.ReadMilliSec());

    t.Start();
    for (int i = 0; i < TIMES; ++i)
        double test = t2.ReadMicroSec();
    t.Stop();
    SQUE_LOG(LT_INFO, "SQUE_Timer::ReadMicroSec(): %f ms", t.ReadMilliSec());

    t.Start();
    for (int i = 0; i < TIMES; ++i)
        double test = t2.ReadNanoSec();
    t.Stop();
    SQUE_LOG(LT_INFO, "SQUE_Timer::ReadNanoSec(): %f ms", t.ReadMilliSec());
}

void LoggingPerformance()
{
    SQUE_Timer t;
    // Logs generate a ton of memory each one, so stick to 1000
    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_LOG(LT_INFO, "Simple Log");
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_LOG Small Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_LOG(LT_INFO, "This is a long log that will put %s to think about the decisions in life and wonder if it was really worth the %d years it took %s %f.2 %b", "Marc Torres Jimenez", 1234, "iasuhfosaiugfh", 123124.124, true);
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_LOG Long Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_PRINT(LT_INFO, "Simple Log");
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_PRINT Small Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

    t.Start();
    for (int i = 0; i < TIMES_S; ++i)
        SQUE_PRINT(LT_INFO, "This is a long log that will put %s to think about the decisions in life and wonder if it was really worth the %d years it took %s %f.2 %b", "Marc Torres Jimenez", 1234, "iasuhfosaiugfh", 123124.124, true);
    t.Stop();
    SQUE_LOG(LT_INFO, "---------------------------------------------------");
    SQUE_LOG(LT_INFO, "SQUE_PRINT Long Log: %f ms", t.ReadMilliSec());
    SQUE_LOG(LT_INFO, "---------------------------------------------------");

}

#endif