#ifndef _FLY_LIB_
#define _FLY_LIB_
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CORE //////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Export Markers
#ifndef FL_API
#   ifdef _WIN32
#       if defined(FL_BUILD_SHARED) /* build dll */
#           define FL_API __declspec(dllexport)
#       elif !defined(FL_BUILD_STATIC) /* use dll */
#           define FL_API __declspec(dllimport)
#       else /* static library */
#           define FL_API
#       endif
#   else
#       define FL_API
#   endif
#endif

#define FL_VERSION_MAJOR 2020
#define FL_VERSION_MINOR 1
#define FL_VERSION ((FL_VERSION_MAJOR << 16) | FL_VERSION_MINOR)

unsigned int FLGetVersion(void);
int FLIsCompatibleDLL(void);

FL_API bool FLYLIB_INIT(/* flags */);
FL_API bool FLYLIB_CLOSE(/* flags */);
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGGER ////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <cstdarg>

#define LOGSIZE 512
typedef enum FlyLogType
{
	LT_INFO = 4,
	LT_WARNING,
	LT_ERROR,
	LT_CRITICAL
	
} FlyLogType;
typedef struct FlyLog
{
	int type = -1;
	FlyLogType lt = LT_INFO;
	char log[LOGSIZE] = {0};
} FlyLog;
FL_API void FlyPrintLog (const char* log, int lt);
FL_API void FLYLOGGER_DumpData();
FL_API bool FLYLOGGER_INIT(bool dumpdata);
FL_API void FLYLOGGER_CLOSE();
FL_API void FLYLOGGER_LOG(FlyLogType lt, const char file[], int line, const char* format, ...);
#define FLYLOG(LogType,format,...) FL_API FLYLOGGER_LOG(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TIMER /////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <atomic>

typedef struct FlyTimer
{
	FlyTimer();
	~FlyTimer();

	void Start();
	void Stop();
	bool IsStopped() const;
	uint16_t ReadMilliSec() const;
	uint32_t ReadMicroSec() const;
	uint32_t ReadNanoSec() const;
	
private:
	uint32_t start_at_ns;
	uint32_t start_at_us;
	uint16_t start_at_ms;

	uint32_t stop_at_ns;
	uint32_t stop_at_us;
	uint16_t stop_at_ms;

	std::atomic<bool> is_stopped;
} FlyTimer;

namespace FLYWINDOW
{

}



#endif // _FLY_LIB_