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

unsigned int FLYLIB_GetVersion(void);
int FLYLIB_IsCompatibleDLL(void);

FL_API bool FLYLIB_Init(/* flags */);
FL_API bool FLYLIB_Close(/* flags */);

enum FLY_BitFlags{
	BITSET1 = 1 << 0,	BITSET17 = 1 << 16,	BITSET33 = 1 << 32,	BITSET49 = 1 << 48,
	BITSET2 = 1 << 1,	BITSET18 = 1 << 17,	BITSET34 = 1 << 33,	BITSET50 = 1 << 49,
	BITSET3 = 1 << 2,	BITSET19 = 1 << 18,	BITSET35 = 1 << 34,	BITSET51 = 1 << 50,
	BITSET4 = 1 << 3,	BITSET20 = 1 << 19,	BITSET36 = 1 << 35,	BITSET52 = 1 << 51,
	BITSET5 = 1 << 4,	BITSET21 = 1 << 20,	BITSET37 = 1 << 36,	BITSET53 = 1 << 52,
	BITSET6 = 1 << 5,	BITSET22 = 1 << 21,	BITSET38 = 1 << 37,	BITSET54 = 1 << 53,
	BITSET7 = 1 << 6,	BITSET23 = 1 << 22,	BITSET39 = 1 << 38,	BITSET55 = 1 << 54,
	BITSET8 = 1 << 7,	BITSET24 = 1 << 23,	BITSET40 = 1 << 39,	BITSET56 = 1 << 55,
	BITSET9 = 1 << 8,	BITSET25 = 1 << 24,	BITSET41 = 1 << 40,	BITSET57 = 1 << 56,
	BITSET10 = 1 << 9,	BITSET26 = 1 << 25,	BITSET42 = 1 << 41,	BITSET58 = 1 << 57,
	BITSET11 = 1 << 10,	BITSET27 = 1 << 26,	BITSET43 = 1 << 42,	BITSET59 = 1 << 58,
	BITSET12 = 1 << 11,	BITSET28 = 1 << 27,	BITSET44 = 1 << 43,	BITSET60 = 1 << 59,
	BITSET13 = 1 << 12,	BITSET29 = 1 << 28,	BITSET45 = 1 << 44,	BITSET61 = 1 << 60,
	BITSET14 = 1 << 13,	BITSET30 = 1 << 29,	BITSET46 = 1 << 45,	BITSET62 = 1 << 61,
	BITSET15 = 1 << 14,	BITSET31 = 1 << 30,	BITSET47 = 1 << 46,	BITSET63 = 1 << 62,
	BITSET16 = 1 << 15,	BITSET32 = 1 << 31,	BITSET48 = 1 << 47,	BITSET64 = 1 << 63
	
};

typedef unsigned short uint16;
typedef unsigned int uint32;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGGER ////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <cstdarg>

#define LOGSIZE 512
typedef enum FLY_LogType
{
	LT_INFO = 4,
	LT_WARNING,
	LT_ERROR,
	LT_CRITICAL
	
} FLY_LogType;
typedef struct FLY_Log
{
	int type = -1;
	FLY_LogType lt = LT_INFO;
	char log[LOGSIZE] = {0};
} FLY_Log;
FL_API void FLYLOGGER_Print(const char* log, int lt);
FL_API void FLYLOGGER_DumpData();
FL_API bool FLYLOGGER_Init(bool dumpdata);
FL_API void FLYLOGGER_Close();
FL_API void FLYLOGGER_Log(FLY_LogType lt, const char file[], int line, const char* format, ...);
#define FLYLOG(LogType,format,...) FL_MACRO FLYLOGGER_Log(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TIMER /////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <atomic>

typedef struct FLY_Timer
{
	FLY_Timer();
	~FLY_Timer();

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
} FLY_Timer;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Flag setting and unsettign using |=FLAG|FLAG and &=(~FLAG | ~FLAG | ~FLAG |...)
typedef uint16_t FLY_WindowFlags; // -> enum FLY_WindowFlags_
enum FLY_WindowFlags_
{
	FLYWINDOW_RESIZABLE = BITSET1,
	FLYWINDOW_DECORATED = BITSET2,
	FLYWINDOW_ALWAYS_TOP = BITSET3,
	FLYWINDOW_MAXIMIZED = BITSET4
};


typedef struct FLY_Window{
	const char* title = "";
	uint16 width=0, height=0;
	FLY_WindowFlags flags;
} FLY_Window;

// Initialization / Global State
FL_API bool FLYDISPLAY_Init(uint16 flags, const char* title = "", uint16 w = 0, uint16 h = 0);
FL_API bool FLYDISPLAY_Close();
FL_API void FLYDISPLAY_SetVSYNC(uint16 vsync_val);

// Control Specific Windows
FL_API void FLYDISPLAY_Resize(uint16 window, uint16 w, uint16 h);
FL_API void FLYDISPLAY_GetSize(uint16 window, uint16* w, uint16* h);
FL_API void FLYDISPLAY_GetAmountWindows(uint16* windows);
FL_API bool FLYDISPLAY_ShouldWindowClose(uint16 window);
FL_API void FLYDISPLAY_CloseWindow(uint16 window);
FL_API void FLYDISPLAY_DestroyWindow(uint16 window);
FL_API bool FLYDISPLAY_OpenWindow(FLY_Window* window = NULL, uint16 monitor = 0);
// Controlling Contexts
FL_API void FLYDISPLAY_Clean();
FL_API void FLYDISPLAY_SwapBuffers();
FL_API void FLYDISPLAY_MakeContextMain(uint16 window);

#endif // _FLY_LIB_