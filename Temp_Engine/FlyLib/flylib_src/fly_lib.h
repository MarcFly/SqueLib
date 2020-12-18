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

#ifdef _MSC_VER
#	define FL_MACRO
#else 
#	define FL_MACRO FL_API
#endif

#define FL_VERSION_MAJOR 2020
#define FL_VERSION_MINOR 1
#define FL_VERSION ((FL_VERSION_MAJOR << 16) | FL_VERSION_MINOR)

unsigned int FLGetVersion(void);
int FLIsCompatibleDLL(void);

FL_API bool FLYLIB_Init(/* flags */);
FL_API bool FLYLIB_Close(/* flags */);
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
FL_API bool FLYLOGGER_Init(bool dumpdata);
FL_API void FLYLOGGER_Close();
FL_API void FLYLOGGER_Log(FlyLogType lt, const char file[], int line, const char* format, ...);
#define FLYLOG(LogType,format,...) FL_MACRO FLYLOGGER_Log(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

FL_API bool FLYDISPLAY_Init(const char* title = "", int w = 0, int h = 0 /*,flags*/);
FL_API bool FLYDISPLAY_Close();
FL_API void FLYDISPLAY_SetVSYN(int vsync_val);
FL_API void FLYDISPLAY_Clean();
FL_API void FLYDISPLAY_SwapBuffers();
FL_API void FLYDISPLAY_Resize(int w, int h);
FL_API void FLYDISPLAY_GetSize(int* w, int* h);
FL_API void FLYDISPLAY_CloseWindowCallback(int window);
FL_API void FLYDISPLAY_DestroyWindow(int window);
FL_API bool FLYDISPLAY_OpenWindow(int width, int height, const char* title, int monitor = 0);
#endif // _FLY_LIB_