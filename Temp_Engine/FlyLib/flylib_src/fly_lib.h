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

enum FLY_BitFlags
{
	BITSET1 = 1 << 0,	BITSET17 = 1 << 16,	// BITSET33 = 1 << 32,	BITSET49 = 1 << 48,
	BITSET2 = 1 << 1,	BITSET18 = 1 << 17,	// BITSET34 = 1 << 33,	BITSET50 = 1 << 49,
	BITSET3 = 1 << 2,	BITSET19 = 1 << 18,	// BITSET35 = 1 << 34,	BITSET51 = 1 << 50,
	BITSET4 = 1 << 3,	BITSET20 = 1 << 19,	// BITSET36 = 1 << 35,	BITSET52 = 1 << 51,
	BITSET5 = 1 << 4,	BITSET21 = 1 << 20,	// BITSET37 = 1 << 36,	BITSET53 = 1 << 52,
	BITSET6 = 1 << 5,	BITSET22 = 1 << 21,	// BITSET38 = 1 << 37,	BITSET54 = 1 << 53,
	BITSET7 = 1 << 6,	BITSET23 = 1 << 22,	// BITSET39 = 1 << 38,	BITSET55 = 1 << 54,
	BITSET8 = 1 << 7,	BITSET24 = 1 << 23,	// BITSET40 = 1 << 39,	BITSET56 = 1 << 55,
	BITSET9 = 1 << 8,	BITSET25 = 1 << 24,	// BITSET41 = 1 << 40,	BITSET57 = 1 << 56,
	BITSET10 = 1 << 9,	BITSET26 = 1 << 25,	// BITSET42 = 1 << 41,	BITSET58 = 1 << 57,
	BITSET11 = 1 << 10,	BITSET27 = 1 << 26,	// BITSET43 = 1 << 42,	BITSET59 = 1 << 58,
	BITSET12 = 1 << 11,	BITSET28 = 1 << 27,	// BITSET44 = 1 << 43,	BITSET60 = 1 << 59,
	BITSET13 = 1 << 12,	BITSET29 = 1 << 28,	// BITSET45 = 1 << 44,	BITSET61 = 1 << 60,
	BITSET14 = 1 << 13,	BITSET30 = 1 << 29,	// BITSET46 = 1 << 45,	BITSET62 = 1 << 61,
	BITSET15 = 1 << 14,	BITSET31 = 1 << 30,	// BITSET47 = 1 << 46,	BITSET63 = 1 << 62,
	BITSET16 = 1 << 15,	BITSET32 = 1 << 31	// BITSET48 = 1 << 47,	BITSET64 = 1 << 63
	
}; // Enums are hard truncated to 32bit by default, just does not throw eror in MSVC
// To get 64bit you have to define the size (enum MyEnum : <integer_type>) but I can' make it work

typedef short int16;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef int int32;

typedef struct float4 { float x, y, z, w;} float4;
typedef struct float3 { float x, y, z;} float3;
typedef struct float2 { float x, y;} float2;


#ifdef ANDROID
#   define USE_EGL
#	define USE_OPENGLES
#elif defined _WIN32 || defined __linux__
#   define USE_GLFW
#	define USE_OPENGL
#endif

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
FL_API void FLY_ConsolePrint(int lt, const char* log);
FL_API void FLYLOGGER_DumpData();
FL_API bool FLYLOGGER_Init(bool dumpdata);
FL_API void FLYLOGGER_Close();
FL_API void FLYLOGGER_Log(FLY_LogType lt, const char file[], int line, const char* format, ...);
#define FLYLOG(LogType,format,...) FL_MACRO FLYLOGGER_Log(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)
FL_API void FLYLOGGER_PrintVargs(FLY_LogType lt, const char file[], int line, const char* format, ...);
#define FLYPRINT(LogType, format,...) FL_MACRO FLYLOGGER_PrintVargs(LogType, __FILE__, __LINE__, format, ##__VA_ARGS__)


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TIMER /////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <atomic>
#include <cstddef>

typedef struct FLY_Timer
{
	FLY_Timer();
	~FLY_Timer() {};

	void Start();
	void Stop();
	void Kill();
	bool IsStopped() const;
	bool IsActive() const;
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
	std::atomic<bool> is_active;
} FLY_Timer;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Flag setting and unsettign using |=FLAG|FLAG and &=(~FLAG | ~FLAG | ~FLAG |...)
typedef uint16_t FLY_WindowFlags; // -> enum FLY_WindowFlags_
enum FLY_WindowFlags_
{
	FLYWINDOW_NOT_RESIZABLE = BITSET1,
	FLYWINDOW_NOT_DECORATED = BITSET2,
	FLYWINDOW_ALWAYS_TOP 	= BITSET3,
	FLYWINDOW_MAXIMIZED 	= BITSET4,
	FLYWINDOW_TO_CLOSE 		= BITSET5,
};


typedef struct FLY_Window
{
	const char* title = "";
	uint16 width=0, height=0;
	FLY_WindowFlags flags;
	int mouse_in = -1;
} FLY_Window;

// Initialization / Global State
FL_API bool FLYDISPLAY_Init(uint16 flags, const char* title = "", uint16 w = 0, uint16 h = 0);
FL_API bool FLYDISPLAY_Close();
FL_API void FLYDISPLAY_SetVSYNC(int16 vsync_val);
FL_API int32 FLYDISPLAY_GetDPIDensity(uint16 window = 0);
// Control Specific Windows
FL_API void FLYDISPLAY_Resize(uint16 window, uint16 w, uint16 h);
FL_API void FLYDISPLAY_GetSize(uint16 window, uint16* w, uint16* h);
FL_API void FLYDISPLAY_GetAmountWindows(uint16* windows);
FL_API void FLYDISPLAY_SetWindowClose(uint16 window);
FL_API bool FLYDISPLAY_ShouldWindowClose(uint16 window);
FL_API void FLYDISPLAY_CloseWindow(uint16 window);
FL_API void FLYDISPLAY_DestroyWindow(uint16 window);
FL_API bool FLYDISPLAY_OpenWindow(FLY_Window* window = NULL, uint16 monitor = 0);
// Controlling Contexts
FL_API void FLYDISPLAY_Clean();
FL_API void FLYDISPLAY_SwapAllBuffers();
FL_API void FLYDISPLAY_MakeContextMain(uint16 window);
// Sending GLFW/EGL/... specifics for other apps to use
struct GLFWwindow;
FL_API GLFWwindow* FLYDISPLAY_RetrieveMainGLFWwindow();

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT /////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

enum FLY_InputState
{
	FLY_UNKNOWN = -1,
	FLY_RELEASED,
	FLY_PRESSED,
	FLY_HELD
};

enum FLY_KeyboardKeys
{
	FLY_KEY_UNKNWON = -1,
	// 1 Initial key
	// MAIN PRINTABLE ASCII CHARACTERS
	FLY_KEY_SPACE			= 32,	FLY_KEY_SINGLE_QUOTE	= 39,	FLY_KEY_ASTERISK		= 42,	FLY_KEY_PLUS			= 43,
	FLY_KEY_COMMA			= 44,	FLY_KEY_DASH			= 45,	FLY_KEY_DOT				= 46,	FLY_KEY_FORW_SLASH		= 47,
	FLY_KEY_ZERO_0			= 48,	FLY_KEY_ONE_1			= 49,	FLY_KEY_TWO_2			= 50,	FLY_KEY_THREE_3			= 51,
	FLY_KEY_FOUR_4			= 52,	FLY_KEY_FIVE_5			= 53,	FLY_KEY_SIX_6			= 54,	FLY_KEY_SEVEN_7			= 55,
	FLY_KEY_EIGHT_8			= 56,	FLY_KEY_NINE_9			= 57,	FLY_KEY_COLON			= 58,	FLY_KEY_SEMI_COLON		= 59,
	FLY_KEY_EQUALS			= 61,	FLY_KEY_GREATER_THAN	= 62,	FLY_KEY_END_QUESTION	= 63,	FLY_KEY_AT				= 64,
	FLY_KEY_UPPER_A			= 65,	FLY_KEY_UPPER_B			= 66,	FLY_KEY_UPPER_C			= 67,	FLY_KEY_UPPER_D			= 68,
	FLY_KEY_UPPER_E			= 69,	FLY_KEY_UPPER_F			= 70,	FLY_KEY_UPPER_G			= 71,	FLY_KEY_UPPER_H			= 72,	
	FLY_KEY_UPPER_I			= 73,	FLY_KEY_UPPER_J			= 74,	FLY_KEY_UPPER_K			= 75,	FLY_KEY_UPPER_L			= 76,
	FLY_KEY_UPPER_M			= 77,	FLY_KEY_UPPER_N			= 78,	FLY_KEY_UPPER_O			= 79,	FLY_KEY_UPPER_P			= 80,
	FLY_KEY_UPPER_Q			= 81,	FLY_KEY_UPPER_R			= 82,	FLY_KEY_UPPER_S			= 83,	FLY_KEY_UPPER_T			= 84,
	FLY_KEY_UPPER_U			= 85,	FLY_KEY_UPPER_V			= 86,	FLY_KEY_UPPER_W			= 87,	FLY_KEY_UPPER_X			= 88,
	FLY_KEY_UPPER_Y			= 89,	FLY_KEY_UPPER_Z			= 90,	FLY_KEY_OPEN_BRACKET	= 91,	FLY_KEY_BACK_SLASH		= 92,	
	FLY_KEY_CLOSE_BRACKET	= 93, 	FLY_KEY_OPEN_ACCENT		= 96,
	
	// 54 keys here

	// MAIN USAGE CHARACTERS
	FLY_KEY_WORLD_1			= 161,	FLY_KEY_WORLD_2		= 162,	FLY_KEY_ESCAPE			= 256,	FLY_KEY_ENTER				= 257,
	FLY_KEY_TAB				= 258,	FLY_KEY_BACKSPACE	= 259,	FLY_KEY_INSERT			= 260,	FLY_KEY_DELETE				= 261,
	FLY_KEY_ARROW_RIGHT		= 262,	FLY_KEY_ARROW_LEFT	= 263,	FLY_KEY_ARROW_DOWN		= 264,	FLY_KEY_ARROW_UP			= 265,
	FLY_KEY_PAGE_UP			= 266,	FLY_KEY_PAGE_DOWN	= 267,	FLY_KEY_HOME			= 268,	FLY_KEY_END					= 269,
	FLY_KEY_CAPS_LOCK		= 281,	FLY_KEY_SCROLL_LOCK	= 281,	FLY_KEY_NUM_LOCK		= 282,	FLY_KEY_PRINT_SCREEN		= 283,
	FLY_KEY_PAUSE			= 284,	FLY_KEY_F1			= 290,	FLY_KEY_F2				= 291,	FLY_KEY_F3					= 292,
	FLY_KEY_F4				= 293,	FLY_KEY_F5			= 294,	FLY_KEY_F6				= 295,	FLY_KEY_F7					= 296,
	FLY_KEY_F8				= 297,	FLY_KEY_F9			= 298,	FLY_KEY_F10				= 299,	FLY_KEY_F11					= 300,
	FLY_KEY_F12				= 301,	FLY_KEY_F13			= 302,	FLY_KEY_F14				= 303,	FLY_KEY_F15					= 304,
	FLY_KEY_F16				= 305,	FLY_KEY_F17			= 306,	FLY_KEY_F18				= 307,	FLY_KEY_F19					= 308,
	FLY_KEY_F20				= 309,	FLY_KEY_F21			= 310,	FLY_KEY_F22				= 311,	FLY_KEY_F23					= 312,
	FLY_KEY_F24				= 313,	FLY_KEY_F25			= 314,	FLY_KEY_KEYPAD_1		= 320,	FLY_KEY_KEYPAD_2			= 321,
	FLY_KEY_KEYPAD_3		= 322,	FLY_KEY_KEYPAD_4	= 323,	FLY_KEY_KEYPAD_5		= 324,	FLY_KEY_KEYPAD_6			= 325,
	FLY_KEY_KEYPAD_7		= 326,	FLY_KEY_KEYPAD_8	= 327,	FLY_KEY_KEYPAD_9		= 328,	FLY_KEY_KEYPAD_DECIMAL		= 329,
	FLY_KEY_KEYPAD_DIVIDE	= 331,	FLY_KEY_KEYPAD_MULT	= 332,	FLY_KEY_KEYPAD_SUBTRACT	= 333,	FLY_KEY_KEYPAD_ADD			= 334,
	FLY_KEY_KEYPAD_ENTER	= 335,	FLY_KEY_KEYPAD_EQUAL= 336,	FLY_KEY_LEFT_SHIFT		= 340,	FLY_KEY_LEFT_CTRL			= 341,
	FLY_KEY_LEFT_ALT		= 342,	FLY_KEY_LEFT_SUPER	= 343,	FLY_KEY_RIGHT_SHIFT		= 344,	FLY_KEY_RIGHT_CTRL			= 345,
	FLY_KEY_RIGHT_ALT		= 347,	FLY_KEY_RIGHT_SUPER	= 348,
	// 66 keys here
	// EXTENDED ASCII CHARACTERS

	// END
	FLY_KEY_MAX 
};
#define NUM_KEYS 121
#define MAX_KEYS 512
#define MAX_MOUSE_BUTTONS 16
#define MAX_POINTERS 10
#define GESTURE_REFRESH 10 // in ms
#define MAX_MIDPOINTS 10

enum FLYINPUT_ACTIONS
{
	FLY_ACTION_UNKNOWN = -1,
	// Button States
	FLY_ACTION_RELEASE,
	FLY_ACTION_PRESS,
	FLY_ACTION_REPEAT,
	
	// Touch Controls
	FLY_ACTION_TAP,
	FLY_ACTION_CLICK,
	FLY_ACTION_DOUBLE_CLICK,
	FLY_ACTION_SWIPE_UP,
	FLY_ACTION_SWIPE_DOWN,
	FLY_ACTION_SWIPE_LEFT,
	FLY_ACTION_SWIPE_RIGHT,
	// Gameplay Actions

	// 
	FLY_ACTION_MAX
};

FL_API void FLYINPUT_Init(uint16 window);
FL_API void FLYINPUT_Process(uint16 window);
FL_API void FLYINPUT_DisplaySoftwareKeyboard(bool show);
FL_API FLYINPUT_ACTIONS FLYINPUT_GetMouseButton(int button);
FL_API void FLYINPUT_GetMousePos(float* x, float* y);
FL_API FLYINPUT_ACTIONS FLYINPUT_EvalGesture();

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING /////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ColorRGBA {
	ColorRGBA(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	float r, g, b, a;
} ColorRGBA;

FL_API void FLYRENDER_ViewportSizeCallback(int width, int height);
FL_API bool FLYRENDER_Init();
FL_API void FLYRENDER_Clear(int clear_flags = NULL, ColorRGBA* color_rgba = NULL);
FL_API const char* FLYRENDER_GetGLSLVer();
FL_API void ForceLoadGL();

#endif // _FLY_LIB_