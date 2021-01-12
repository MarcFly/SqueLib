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

#ifdef ANDROID
#   define USE_EGL
#	define USE_OPENGLES
#elif defined _WIN32 || defined __linux__
#   define USE_GLFW
#	define USE_OPENGL
#endif

#ifdef _WIN32
#   define FOLDER_ENDING '\\'
#else
#   define FOLDER_ENDING '/'
#endif

#include <fly_remap_macros.h>

FL_API unsigned int FLYLIB_GetVersion(void);
FL_API int FLYLIB_IsCompatibleDLL(void);

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

#define SET_FLAG(n, f) ((n) |= (f)) 
#define CLR_FLAG(n, f) ((n) &= ~(f))
#define APPLY_MASK(n, f) ((n) &= (f))
#define TGL_FLAG(n, f) ((n) ^= (f)) 
#define CHK_FLAG(n, f) (((n) & (f)) > 0)

typedef unsigned short uint16;
typedef unsigned int uint32;
typedef short int16;
typedef int int32;
typedef long long double64;
typedef unsigned long long udouble64;
typedef unsigned char uchar;

typedef void(*VoidFun)();

typedef struct float4 
{
	float4() : x(0), y(0), z(0), w(0) {};
	float4(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {};
	float x, y, z, w;
} float4;
typedef struct float3 
{ 
	float3() : x(0), y(0), z(0) {};
	float3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {};
	float x, y, z;
} float3;
typedef struct float2 
{ 
	float2() : x(0), y(0) {};
	float2(float x_, float y_) : x(x_), y(y_) {}; 
	float x, y;
} float2;

typedef struct int4
{
	int4() : x(0), y(0), z(0), w(0) {};
	int4(int32 x_, int32 y_, int32 z_, int32 w_) : x(x_), y(y_), z(z_), w(w_) {};
	int32 x, y, z, w;
} int4;
typedef struct int3
{
	int3() : x(0), y(0), z(0) {};
	int3(int32 x_, int32 y_, int32 z_) : x(x_), y(y_), z(z_) {};
	int32 x, y, z;
} int3;
typedef struct int2
{
	int2() : x(0), y(0) {};
	int2(int32 x_, int32 y_) : x(x_), y(y_) {};
	int32 x, y;
} int2;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGGER ////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <cstdarg>
#include <cstddef>

#define LOGSIZE 1024
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

#include <atomic>

typedef struct FLY_Timer
{
	FL_API FLY_Timer();
	
	FL_API void Start();
	FL_API void Stop();
	FL_API void Kill();
	FL_API bool IsStopped();
	FL_API bool IsActive();
	FL_API uint32 ReadMilliSec();
	FL_API uint32 ReadMicroSec();
	FL_API uint32 ReadNanoSec();
	
private:
	udouble64 start_at_ms;
	udouble64 start_at_ns;
	udouble64 start_at_us;
	
	udouble64 stop_at_ms;
	udouble64 stop_at_ns;
	udouble64 stop_at_us;

	std::atomic<bool> is_stopped;
	std::atomic<bool> is_active;
} FLY_Timer;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Flag setting and unsettign using |=FLAG|FLAG and &=(~FLAG | ~FLAG | ~FLAG |...)
enum FLY_WindowFlags
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
	uint16 flags;
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

enum FLYINPUT_Actions
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

// Structs for Handling Data

typedef void(*KeyCallback)(int32 code, int32 state);
void EmptyKey(int32 code, int32 state);
typedef struct FLY_Key
{
	//int key;
	int prev_state = -1;
	int state = -1;
	KeyCallback callback = EmptyKey;
} FLY_Key;

typedef void(*MouseFloatCallback)(float x, float y);
void EmptyMouseFloatCallback(float x, float y);
typedef struct FLY_Mouse
{
	float prev_y = INT32_MAX, prev_x = INT32_MAX;
	float x = INT32_MAX, y = INT32_MAX;
	MouseFloatCallback pos_callback = EmptyMouseFloatCallback;

	float prev_scrollx = INT32_MAX, prev_scrolly = INT32_MAX;
	float scrollx = INT32_MAX, scrolly = INT32_MAX;
	MouseFloatCallback scroll_callback = EmptyMouseFloatCallback;

	FLY_Key mbuttons[MAX_MOUSE_BUTTONS];

} FLY_Mouse;


// Touch Display Oriented - But will implement mouse based gestures and callback based key gestures
typedef struct FLY_Gesture
{
	FLY_Timer timer;

	float start_x, start_y;
	float midpoints[MAX_MIDPOINTS][2];
	float end_x, end_y;

	float refresh_bucket;
} FLY_Gesture;

typedef struct FLY_Pointer
{
	bool active = false;
	int32_t id;
	FLY_Timer timer;
	FLY_Gesture gesture;

} FLY_Pointer;

// Initialization / State Control Functions
FL_API bool FLYINPUT_Init(uint16 window);
FL_API bool FLYINPUT_Close();
FL_API void FLYINPUT_Process(uint16 window);
FL_API void FLYINPUT_DisplaySoftwareKeyboard(bool show);

// Getters
FL_API FLYINPUT_Actions FLYINPUT_GetMouseButton(int button);
FL_API void FLYINPUT_GetMousePos(float* x, float* y);
FL_API void FLYINPUT_GetMouseWheel(float* v = NULL, float* h = NULL);
FL_API FLYINPUT_Actions FLYINPUT_EvalGesture();
FL_API int FLYINPUT_GetCharFromBuffer();

// Callback Setters
FL_API void FLYINPUT_AddOnResumeCallback(VoidFun fn);
FL_API void FLYINPUT_AddOnGoBackgroundCallback(VoidFun fn);
FL_API void FLYINPUT_SetKeyCallback(KeyCallback fly_key_fn);
FL_API void FLYINPUT_SetMouseCallbacks(MouseFloatCallback position, MouseFloatCallback scroll);
FL_API void FLYINPUT_SetMouseButtonCallbacks(int button, KeyCallback key_callback);

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING /////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct ColorRGBA {
	ColorRGBA(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
	float r, g, b, a;
} ColorRGBA;

typedef struct FLY_RenderState
{
	bool backed_up = false;
	int32 bound_texture, active_texture_unit;
	int32 program, vertex_array_buffer, element_array_buffer;
	int32 attribute_object;
	int32 blend_equation_rgb, blend_equation_alpha;
	
	bool blend_func_separate = false;
	int32 blend_func_src_rgb, blend_func_dst_rgb;
	int32 blend_func_src_alpha, blend_func_dst_alpha;

	int4 viewport;
	int4 scissor_box;
	int2 polygon_mode;
	bool blend, cull_faces, depth_test, scissor_test;

	FL_API void SetUp();
	FL_API void BackUp();
} FLY_RenderState;
FL_API void CheckForRenderErrors(const char* file, int line);
#define FLY_CHECK_RENDER_ERRORS() FL_MACRO CheckForRenderErrors(__FILE__, __LINE__)
FL_API void FLYRENDER_ChangeViewPortSize(int width, int height);
FL_API bool FLYRENDER_Init();
FL_API void FLYRENDER_Close();
FL_API void FLYRENDER_Clear(int clear_flags = NULL, ColorRGBA* color_rgba = NULL);
FL_API const char* FLYRENDER_GetGLSLVer();

// Function Passtrhough of ones I don't know
FL_API void FLYRENDER_Scissor(int x, int y, int w, int h);

// Render Pipeline
typedef struct FLY_VertAttrib
{
	int32 id;
	const char* name;
	int32 var_type;
	bool normalize;
	uint16 var_size;
	uint16 num_comp;
	uint32 offset;
	uint16 location;
	// Initialization
	FL_API void SetName(const char* str);
	FL_API void SetVarType(int32 var);
	FL_API void SetNumComponents(uint16 num);
	FL_API void SetNormalize(bool norm);
	FL_API void SetOffset(uint32 offset_bytes);
	FL_API void SetId(int32 id_);
	FL_API void EnableAsAttribute(int32 prog_id);
	// Getters
	FL_API uint16 GetSize() const;

	// Usage
	FL_API void SetAttribute(uint16 vert_size) const;
	FL_API void SetLocation(int32 pos, uint16 vert_size);
} FLY_VertAttrib;

#include <vector> 
// Maybe Swap to a custom array handler for specific sizes
// I want to hav e afast search on less than 100 objects, probably a full array is good enough
typedef struct FLY_Mesh
{	
	int32 draw_mode = FLY_STATIC_DRAW;
	uint32 attribute_object = 0; // VAO in OpenGL, index to holder of attributes
	
	// Vertex Data
	uint32 vert_id = 0;
	uint16 num_verts = 0;
	void* verts = NULL;
	std::vector<FLY_VertAttrib*> attributes;
	
	// Indices for the buffer
	uint32 index_id		= 0;
	uint16 num_index	= 0;
	uint32 index_var	= FLY_UINT; // Default 4 because generally used uint, but ImGui Uses 2 Byte indices
	uint16 index_var_size = 0;
	void* indices		= nullptr;

	// add other parts of the buffer

	// Initialization
	FL_API void SetDrawMode(int32 draw_mode);
	FL_API void SetIndexVarType(int32 var);
	FL_API void Init();

	// Location and Vertex attributes
	FL_API FLY_VertAttrib* AddAttribute(FLY_VertAttrib* attribute = NULL);
	FL_API void SetOffsetsInOrder();
	FL_API void EnableAttributesForProgram(int32 prog_id);
	

	// Getters
	FL_API uint16 GetVertSize();	
	FL_API uint16 GetAttribSize(const char* name) const;

	// Usage
	FL_API void Bind();
	FL_API void BindNoIndices();
	FL_API void SetAttributes();
	FL_API void SetLocationsInOrder();
	FL_API void SendToGPU();

	// CleanUp
	FL_API ~FLY_Mesh();
	FL_API void CleanUp();
} FLY_Mesh;

typedef struct FLY_TexAttrib
{
	int32 id = 0;
	int32 var_type = 0;
	void* data = NULL;

	// Initialization
	FL_API void Set(int32 id_, int32 var_, void* data_);
	FL_API void SetID(int32 id_);
	FL_API void SetVarType(int32 var);
	FL_API void SetData(void* data_);

	// Usage
	FL_API void SetParameter(int32 tex_id);

} FLY_TexAttrib;
typedef struct FLY_Texture2D
{
	uint32 id		= UINT32_MAX;
	int32 format	= 0;
	int32 w = 0, h = 0;
	int32 channel_num = 0;
	void* pixels	= NULL;
	std::vector<FLY_TexAttrib*> attributes;

	// Initialization
	FL_API void Init(int32 tex_format);
	FL_API FLY_TexAttrib* AddParameter(FLY_TexAttrib* tex_attrib = NULL);
	FL_API void SetParameters();
	
	// Usage
	FL_API void Bind();
	FL_API void BindToUnit(int32 texture_unit);
	FL_API void SendToGPU();

	// CleanUp
	FL_API ~FLY_Texture2D();
	FL_API void CleanUp();
} FLY_Texture2D;

FL_API void FLYRENDER_ActiveTexture(int32 texture_id);
FL_API void FLYRENDER_BindExternalTexture(int tex_type, uint32 id);
FL_API void FLYRENDER_BindSampler(int32 texture_locator, int32 sampler_id);

// FLY_Texture3D??...
typedef struct FLY_Texture3D
{
	uint32 id = 0;
	int32 format = 0;
	int w, h;
	uchar** pixels = nullptr;

// Cp[y from FLY_Texture2D
} FLY_Texture3D;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADERS ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct FLY_Shader
{
	int32 id = 0;
	int32 type;
	uint16 lines = 0;
	const char** source;

	// Shader Usage Functions
	FL_API void Compile();

} FLY_Shader;

FL_API FLY_Shader* FLYSHADER_Create(int32 type, uint16 strs, const char** file, bool raw_string = false);
FL_API void FLYSHADER_CheckCompileLog(FLY_Shader* fly_shader);

typedef struct FLY_Uniform
{
	FLY_Uniform() {};
	FLY_Uniform(const char*);
	int32 id = INT32_MAX;
	const char* name = "";
} FLY_Uniform;

#include <vector>
typedef struct FLY_Program
{
	int32 id = 0;
	FLY_Shader* vertex_s = nullptr;
	FLY_Shader* fragment_s = nullptr;
	std::vector<FLY_Uniform*> uniforms;
	std::vector<FLY_VertAttrib*> attributes;
	
	// Call before doing anything prolly
	FL_API void Init();

	// Prepare the Program
	// FL_API FLY_Shader* AddShadder(int32 type,... initializers);
	FL_API void AttachShader(FLY_Shader** fly_shader); // Obtains ownership of the shader 
	FL_API void CompileShaders();
	FL_API void Link();
	
	// Drawing
	FL_API void Use();
	FL_API void DrawIndices(FLY_Mesh* mesh, int32 offset_bytes = 0, int32 count = 0);
	FL_API void DrawRawVertices(FLY_Mesh* mesh, int32 count = 0);

	// Attributes
	FL_API void GiveAttributesFromMesh(FLY_Mesh* fly_mesh);
	FL_API FLY_VertAttrib* AddAttribute(FLY_VertAttrib* attr);
	FL_API uint16 GetAttribByteSize() const;
	FL_API void SetAttribLocations();
	FL_API void EnableOwnAttributes();
	
	// Passing Uniforms
	FL_API void DeclareUniform(const char*);
	FL_API int32 GetUniformLocation(const char* name) const;

	FL_API void SetBool(const char* name, bool value) const;
	FL_API void SetInt(const char* name, int value) const;
	FL_API void SetFloat(const char* name, float value) const;
	FL_API void SetFloat2(const char* name, float2 value) const;
	FL_API void SetFloat3(const char* name, float3 value) const;
	FL_API void SetFloat4(const char* name, float4 value) const;
	// ... add a matrix/array passer...
	FL_API void SetMatrix4(const char* name, const float* matrix, uint16 number = 1, bool transpose = false) const;

	// CleanUp
	FL_API void CleanUp();
	FL_API ~FLY_Program();
	

	
} FLY_Program;


// I will send the hwole pointer because i don't know how other libs will try to access data
// OpenGL marks by ids and it would be better to just sent the id, but other may differ and require more
FL_API FLY_Program* FLYSHADER_CreateProgram(uint16 layout_flags);
FL_API void FLYRENDER_CheckProgramLog(FLY_Program* fly_program);

#endif // _FLY_LIB_