#ifndef _SQUE_LIB_																														
#define _SQUE_LIB_																														
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CORE ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Export Markers //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef SQ_API																															
#   ifdef _WIN32																														
#       if defined(SQ_BUILD_SHARED) /* build dll */																						
#           define SQ_API __declspec(dllexport)																							
#       elif !defined(SQ_BUILD_STATIC) /* use dll */																					
#           define SQ_API __declspec(dllimport)																							
#       else /* static library */																										
#           define SQ_API																												
#       endif																															
#   else																																
#       define SQ_API																													
#   endif																																
#endif																																	
																																		
// Macro Exporter, some macros that wrap functions are a bit stupid in MSC																
#ifdef _MSC_VER																															
#	define SQ_MACRO																														
#else 																																	
#	define SQ_MACRO SQ_API																												
#endif																																	
																																		
																																		
																																		
// OS Specific Defs ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef _WIN32																															
#   define FOLDER_ENDING '\\'	
#	define FE '\\'
#else																																	
#   define FOLDER_ENDING '/'	
#	define FE '/'
#endif																																	
																																		
#if defined(ANDROID)	// This Ifdef has to be revised, because I am supposed to include more rendering apis...								
#   define USE_EGL																														
#	define USE_OPENGLES																													
#elif defined _WIN32 || defined __linux__																								
#	define USE_OPENGL
#   define USE_GLFW	
#	if defined(_WIN32)
#		define WIN32_LEAN_AND_MEAN // Just in case
#       define GLFW_EXPOSE_NATIVE_WIN32
#	else
#	endif																						
#endif																																	

// Includes from own libs for organization /////////////////////////////////////////////////////////////////////////////////////////////
#include <sque_remap_macros.h>																											
#include <sque_simple_types.h>																											
#include <stdint.h>
// Currently all math with go through glm, I don't like but I don't have an easy drop in replacement
#include<glm.hpp>
// Initialization / State Control //////////////////////////////////////////////////////////////////////////////////////////////////////
#define SQUE_VERSION_MAJOR 2020																											
#define SQUE_VERSION_MINOR 1																												
#define SQUE_VERSION ((SQUE_VERSION_MAJOR << 16) | SQUE_VERSION_MINOR)																		

// SQUE_LIB should not have an init/close
// User should do that with specifics per module really
SQ_API void SQUE_LIB_Init(const char* app_name, int32_t flags);																									
SQ_API void SQUE_LIB_Close();																									
SQ_API unsigned int SQUE_LIB_GetVersion();																							
SQ_API int SQUE_LIB_IsCompatibleDLL();		

SQ_API int SQUE_VarGetSize(int type_macro);
SQ_API void SQUE_ConsolePrint(int lt, const char* log);
SQ_API void SQUE_PrintVargs(SQUE_LogType lt, const char file[], int line, const char* format, ...);
#define SQUE_PRINT(LogType, format,...) SQ_MACRO SQUE_PrintVargs(LogType, __FILE__, __LINE__, format, ##__VA_ARGS__)				

// Permissions /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API int SQUE_AskPermissions(const char* permission_name);

// Callback Setters - Flow Management //////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API VoidFun SQUE_AddOnResumeCallback(VoidFun fn);																				
SQ_API VoidFun SQUE_AddOnGoBackgroundCallback(VoidFun fn);																			
SQ_API void SQUE_Sleep(uint32_t ms);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGGER //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <cstdarg>																														
#include <cstddef>																														
																																		
// Types / Strucs //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOGSIZE 1024																													
																														
typedef struct SQUE_Log																													
{																																		
	int type = -1;																														
	SQUE_LogType lt = LT_INFO;																											
	char log[LOGSIZE] = {0};																											
} SQUE_Log;																																
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API bool SQUE_LOGGER_Init(bool dumpdata);																								
SQ_API void SQUE_LOGGER_Close();																											
																																																														
// Function Usage //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_ConsolePrint(int lt, const char* log);																																														
SQ_API void SQUE_LOGGER_Log(SQUE_LogType lt, const char file[], int line, const char* format, ...);																		
SQ_API void SQUE_LOGGER_DumpData();

// EASY Usage //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define SQUE_LOG(LogType,format,...) SQ_MACRO SQUE_LOGGER_Log(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)								
																																		
																																		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TIMER ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
#include <atomic>																														
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct SQUE_Timer																												
{																																		
	SQ_API SQUE_Timer();																													
																																		
	SQ_API void Start();																												
	SQ_API void Stop();																													
	SQ_API void Kill();																													
	SQ_API bool IsStopped() const;																											
	SQ_API bool IsActive() const;																												
	SQ_API double ReadMilliSec() const;																										
	SQ_API double ReadMicroSec() const;																										
	SQ_API double ReadNanoSec() const;																										
																																		
private:																																
	double start_at_ms;																													
	double start_at_ns;																													
	double start_at_us;																													
																																		
	double stop_at_ms;																													
	double stop_at_ns;																													
	double stop_at_us;																													
																																		
	std::atomic<bool> is_stopped;																										
	std::atomic<bool> is_active;																										
} SQUE_Timer;																															
																																		
																																		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Flag setting and unsettign using FLAG MACROS in sque_simple_types.h 																																	
enum SQUE_WindowFlags
{
	SQUE_WINDOW_MOUSE_IN = BITSET1,
	SQUE_WINDOW_TO_MAXIMIZE = BITSET4,
	SQUE_WINDOW_TO_CLOSE = BITSET5,
	
};																																	
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_DISPLAY_Init();
SQ_API void SQUE_DISPLAY_Close();																											
SQ_API void SQUE_DISPLAY_SetVSYNC(int16_t vsync_val);																						
SQ_API int32_t SQUE_DISPLAY_GetDPIDensity(uint16_t window = 0);																				
SQ_API void SQUE_DISPLAY_GetMainDisplaySize(uint16_t& w, uint16_t& h);

																																		
// Control Specific Windows ////////////////////////////////////////////////////////////////////////////////////////////////////////////		
// I don't like having user setup a SQUE_Window directly
SQ_API void SQUE_DISPLAY_NextWindow_WindowHints(int32_t* options, int32_t size);
SQ_API void SQUE_DISPLAY_NextWindow_ContextHints(int32_t* options, int32_t size);
SQ_API void SQUE_DISPLAY_NextWindow_BufferHints(int32_t* options, int32_t size);

SQ_API void SQUE_DISPLAY_OpenWindow(const char* title, int32_t width=0, int32_t height=0, uint16_t monitor = 0);
SQ_API bool SQUE_DISPLAY_ShouldWindowClose(uint16_t window);

// Setters
SQ_API void SQUE_DISPLAY_SetWindowClose(uint16_t window);																																									
SQ_API uint16_t SQUE_DISPLAY_CloseWindow(uint16_t window);																																											
SQ_API void SQUE_DISPLAY_ResizeWindow(uint16_t window, uint16_t w, uint16_t h);
SQ_API void SQUE_DISPLAY_UpdateNativeWindowSize(uint16_t window);
// Getters
SQ_API uint16_t SQUE_DISPLAY_GetAmountWindows();
SQ_API void SQUE_DISPLAY_GetWindowPos(uint16_t window, int32_t& x, int32_t& y);																	
SQ_API void SQUE_DISPLAY_GetWindowSize(uint16_t window, int32_t* w, int32_t* h);
SQ_API void SQUE_DISPLAY_GetViewportSize(uint16_t window, int32_t* w, int32_t* h);
SQ_API void* SQUE_DISPLAY_GetPlatformWindowHandle(uint16_t window);

// Controlling Contexts ////////////////////////////////////////////////////////////////////////////////////////////////////////////////																										
SQ_API void SQUE_DISPLAY_SwapBuffer(uint16_t window);
SQ_API void SQUE_DISPLAY_SwapAllBuffers();																								
SQ_API void SQUE_DISPLAY_MakeContextMain(uint16_t window);
SQ_API ResizeCallback SQUE_DISPLAY_SetViewportResizeCallback(ResizeCallback viewport_cb);
SQ_API ViewportSizeCallback SQUE_DISPLAY_SetViewportSizeCallback(ViewportSizeCallback viewport_size_cb);
																																		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum SQUE_InputState
{																																		
	SQUE_UNKNOWN = -1,																													
	SQUE_RELEASED,																														
	SQUE_PRESSED,																														
	SQUE_HELD																															
};																																		
																																		
enum SQUE_KeyboardKeys
{																																		
	SQUE_KEY_UNKNWON = -1,																												
	// 1 Initial key																													
	// MAIN PRINTABLE ASCII CHARACTERS																									
	SQUE_KEY_SPACE			= 32,	SQUE_KEY_SINGLE_QUOTE	= 39,	SQUE_KEY_ASTERISK		= 42,	SQUE_KEY_PLUS			= 43,		
	SQUE_KEY_COMMA			= 44,	SQUE_KEY_DASH			= 45,	SQUE_KEY_DOT				= 46,	SQUE_KEY_FORW_SLASH		= 47,		
	SQUE_KEY_ZERO_0			= 48,	SQUE_KEY_ONE_1			= 49,	SQUE_KEY_TWO_2			= 50,	SQUE_KEY_THREE_3			= 51,		
	SQUE_KEY_FOUR_4			= 52,	SQUE_KEY_FIVE_5			= 53,	SQUE_KEY_SIX_6			= 54,	SQUE_KEY_SEVEN_7			= 55,		
	SQUE_KEY_EIGHT_8			= 56,	SQUE_KEY_NINE_9			= 57,	SQUE_KEY_COLON			= 58,	SQUE_KEY_SEMI_COLON		= 59,		
	SQUE_KEY_EQUALS			= 61,	SQUE_KEY_GREATER_THAN	= 62,	SQUE_KEY_END_QUESTION	= 63,	SQUE_KEY_AT				= 64,		
	SQUE_KEY_UPPER_A			= 65,	SQUE_KEY_UPPER_B			= 66,	SQUE_KEY_UPPER_C			= 67,	SQUE_KEY_UPPER_D			= 68,		
	SQUE_KEY_UPPER_E			= 69,	SQUE_KEY_UPPER_F			= 70,	SQUE_KEY_UPPER_G			= 71,	SQUE_KEY_UPPER_H			= 72,		
	SQUE_KEY_UPPER_I			= 73,	SQUE_KEY_UPPER_J			= 74,	SQUE_KEY_UPPER_K			= 75,	SQUE_KEY_UPPER_L			= 76,		
	SQUE_KEY_UPPER_M			= 77,	SQUE_KEY_UPPER_N			= 78,	SQUE_KEY_UPPER_O			= 79,	SQUE_KEY_UPPER_P			= 80,		
	SQUE_KEY_UPPER_Q			= 81,	SQUE_KEY_UPPER_R			= 82,	SQUE_KEY_UPPER_S			= 83,	SQUE_KEY_UPPER_T			= 84,		
	SQUE_KEY_UPPER_U			= 85,	SQUE_KEY_UPPER_V			= 86,	SQUE_KEY_UPPER_W			= 87,	SQUE_KEY_UPPER_X			= 88,		
	SQUE_KEY_UPPER_Y			= 89,	SQUE_KEY_UPPER_Z			= 90,	SQUE_KEY_OPEN_BRACKET	= 91,	SQUE_KEY_BACK_SLASH		= 92,		
	SQUE_KEY_CLOSE_BRACKET	= 93, 	SQUE_KEY_OPEN_ACCENT		= 96,																		
																																		
	// 54 keys here																														
																																		
	// MAIN USAGE CHARACTERS																											
	SQUE_KEY_WORLD_1			= 161,	SQUE_KEY_WORLD_2		= 162,	SQUE_KEY_ESCAPE			= 256,	SQUE_KEY_ENTER				= 257,		
	SQUE_KEY_TAB				= 258,	SQUE_KEY_BACKSPACE	= 259,	SQUE_KEY_INSERT			= 260,	SQUE_KEY_DELETE				= 261,		
	SQUE_KEY_ARROW_RIGHT		= 262,	SQUE_KEY_ARROW_LEFT	= 263,	SQUE_KEY_ARROW_DOWN		= 264,	SQUE_KEY_ARROW_UP			= 265,		
	SQUE_KEY_PAGE_UP			= 266,	SQUE_KEY_PAGE_DOWN	= 267,	SQUE_KEY_HOME			= 268,	SQUE_KEY_END					= 269,		
	SQUE_KEY_CAPS_LOCK		= 281,	SQUE_KEY_SCROLL_LOCK	= 281,	SQUE_KEY_NUM_LOCK		= 282,	SQUE_KEY_PRINT_SCREEN		= 283,		
	SQUE_KEY_PAUSE			= 284,	SQUE_KEY_F1			= 290,	SQUE_KEY_F2				= 291,	SQUE_KEY_F3					= 292,		
	SQUE_KEY_F4				= 293,	SQUE_KEY_F5			= 294,	SQUE_KEY_F6				= 295,	SQUE_KEY_F7					= 296,		
	SQUE_KEY_F8				= 297,	SQUE_KEY_F9			= 298,	SQUE_KEY_F10				= 299,	SQUE_KEY_F11					= 300,		
	SQUE_KEY_F12				= 301,	SQUE_KEY_F13			= 302,	SQUE_KEY_F14				= 303,	SQUE_KEY_F15					= 304,		
	SQUE_KEY_F16				= 305,	SQUE_KEY_F17			= 306,	SQUE_KEY_F18				= 307,	SQUE_KEY_F19					= 308,		
	SQUE_KEY_F20				= 309,	SQUE_KEY_F21			= 310,	SQUE_KEY_F22				= 311,	SQUE_KEY_F23					= 312,		
	SQUE_KEY_F24				= 313,	SQUE_KEY_F25			= 314,	SQUE_KEY_KEYPAD_1		= 320,	SQUE_KEY_KEYPAD_2			= 321,		
	SQUE_KEY_KEYPAD_3		= 322,	SQUE_KEY_KEYPAD_4	= 323,	SQUE_KEY_KEYPAD_5		= 324,	SQUE_KEY_KEYPAD_6			= 325,		
	SQUE_KEY_KEYPAD_7		= 326,	SQUE_KEY_KEYPAD_8	= 327,	SQUE_KEY_KEYPAD_9		= 328,	SQUE_KEY_KEYPAD_DECIMAL		= 329,		
	SQUE_KEY_KEYPAD_DIVIDE	= 331,	SQUE_KEY_KEYPAD_MULT	= 332,	SQUE_KEY_KEYPAD_SUBTRACT	= 333,	SQUE_KEY_KEYPAD_ADD			= 334,		
	SQUE_KEY_KEYPAD_ENTER	= 335,	SQUE_KEY_KEYPAD_EQUAL= 336,	SQUE_KEY_LEFT_SHIFT		= 340,	SQUE_KEY_LEFT_CTRL			= 341,		
	SQUE_KEY_LEFT_ALT		= 342,	SQUE_KEY_LEFT_SUPER	= 343,	SQUE_KEY_RIGHT_SHIFT		= 344,	SQUE_KEY_RIGHT_CTRL			= 345,		
	SQUE_KEY_RIGHT_ALT		= 347,	SQUE_KEY_RIGHT_SUPER	= 348,																			
	// 66 keys here																														
	// EXTENDED ASCII CHARACTERS																										
																																		
	// END																																
	SQUE_KEY_MAX 																														
};																																		
#define NUM_KEYS 121																													
#define MAX_KEYS 512																													
#define MAX_MOUSE_BUTTONS 16																											
#define MAX_POINTERS 10																													
#define GESTURE_REFRESH 10 // in ms																										
#define MAX_MIDPOINTS 10																												
																																		
enum SQUE_INPUT_Actions																													
{																																		
	SQUE_ACTION_UNKNOWN = -1,																											
	// Button States																													
	SQUE_ACTION_RELEASE,																													
	SQUE_ACTION_PRESS,																													
	SQUE_ACTION_REPEAT,																													
																																		
	// Touch Controls																													
	SQUE_ACTION_TAP,																														
	SQUE_ACTION_CLICK,																													
	SQUE_ACTION_DOUBLE_CLICK,																											
	SQUE_ACTION_SWIPE_UP,																												
	SQUE_ACTION_SWIPE_DOWN,																												
	SQUE_ACTION_SWIPE_LEFT,																												
	SQUE_ACTION_SWIPE_RIGHT,																												
	// Gameplay Actions																													
																																		
	// 																																	
	SQUE_ACTION_MAX																														
};																																		
																																		
void DebugKey(int32_t code, int32_t state);																									
typedef struct SQUE_Key																													
{																																		
	//int key;																															
	int prev_state = -1;																												
	int state = -1;																														
	KeyCallback callback = DebugKey;																									
} SQUE_Key;																																
																																		
																																		
void DebugMouseFloatCallback(float x, float y);																							
typedef struct SQUE_Mouse																												
{																																		
	float prev_y = INT32_MAX, prev_x = INT32_MAX;																						
	float x = INT32_MAX, y = INT32_MAX;																									
	MouseFloatCallback pos_callback = DebugMouseFloatCallback;																			
																																		
	float prev_scrollx = INT32_MAX, prev_scrolly = INT32_MAX;																			
	float scrollx = INT32_MAX, scrolly = INT32_MAX;																						
	MouseFloatCallback scroll_callback = DebugMouseFloatCallback;																		
																																		
	SQUE_Key mbuttons[MAX_MOUSE_BUTTONS];																								
																																		
} SQUE_Mouse;																															
																																		
																																		
// Touch Display Oriented - But will implement mouse based gestures and callback based key gestures	////////////////////////////////////
typedef struct SQUE_Gesture																												
{																																		
	SQUE_Timer timer;																													
																																		
	float start_x, start_y;																												
	float midpoints[MAX_MIDPOINTS][2];																									
	float end_x, end_y;																													
																																		
	float refresh_bucket;																												
} SQUE_Gesture;																															
																																		
typedef struct SQUE_Pointer																												
{																																		
	bool active = false;																												
	int32_t id;																															
	SQUE_Timer timer;																													
	SQUE_Gesture gesture;																												
																																		
} SQUE_Pointer;																															
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_INPUT_Init();
SQ_API void SQUE_INPUT_InitForWindow(uint16_t window);
SQ_API bool SQUE_INPUT_Close();																											
SQ_API void SQUE_INPUT_Process(uint16_t window);																							
SQ_API void SQUE_INPUT_DisplaySoftwareKeyboard(bool show);																				
															
// Usage / Utilities ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_INPUT_DisplaySoftwareKeyboard(bool show);
SQ_API SQUE_INPUT_Actions SQUE_INPUT_DetectGesture(SQUE_Pointer& p);
SQ_API SQUE_INPUT_Actions SQUE_INPUT_EvalGesture();

// Setters /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_INPUT_SetMousePos(float x, float y);																						

// Getters /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API SQUE_INPUT_Actions SQUE_INPUT_GetMouseButton(int button);
SQ_API void SQUE_INPUT_GetMousePos(float* x, float* y);
SQ_API void SQUE_INPUT_GetMouseWheel(float* v = NULL, float* h = NULL);
SQ_API int SQUE_INPUT_GetCharFromBuffer();

// Callback Setters ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API KeyCallback SQUE_INPUT_SetKeyCallback(KeyCallback sque_key_fn);																		
SQ_API MouseFloatCallback SQUE_INPUT_SetMousePosCallback(MouseFloatCallback position);													
SQ_API MouseFloatCallback SQUE_INPUT_SetMouseScrollCallback(MouseFloatCallback scroll);													
SQ_API KeyCallback SQUE_INPUT_SetMouseButtonCallbacks(int button, KeyCallback key_callback);												
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESHES //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct SQUE_VertAttrib
{
	// Constructors / Destructors
	SQ_API SQUE_VertAttrib();
	SQ_API SQUE_VertAttrib(const char* name_, int32_t var_type, bool normalize, uint16_t num_components);

	// Variables
	int32_t id;
	int32_t var_type;
	uint16_t num_comp;
	bool normalize;
	uint16_t var_size;
	uint16_t vert_size;
	uint16_t offset;
	const char* name;

	// Usage functions	
	SQ_API uint16_t GetSize() const;

} SQUE_VertAttrib;

#include <vector> 																														
// Maybe Swap to a custom array handler for specific sizes																				
// I want to hav e afast search on less than 100 objects, probably a full array is good enough											
struct SQUE_Program;
typedef struct SQUE_Mesh
{
// Constructors / Destructors
	SQ_API SQUE_Mesh();
	SQ_API SQUE_Mesh(int32_t draw_config, int32_t draw_mode, int32_t index_var);

	SQ_API ~SQUE_Mesh();

// Variables
	int32_t draw_config;
	int32_t draw_mode = SQUE_STATIC_DRAW;
	uint32_t attribute_object = 0;

	// Vertex Data																														
	uint32_t vert_id = 0;
	uint32_t num_verts = 0;
	void* verts = NULL;
	// Indices for the buffer																											
	uint32_t index_id = 0;
	uint16_t num_index = 0;
	uint32_t index_var = SQUE_UINT; // Default 4 because generally used uint, but ImGui Uses 2 Byte indices								
	uint16_t index_var_size = 0;
	void* indices = nullptr;

// Usage Functions
	// Changing the data dynamically (good for stream/dynamic draw)
	SQ_API void ChangeVertData(int32_t num_verts_, void* verts_);
	SQ_API void ChangeIndexData(int32_t num_index_, void* indices_);

	// Location and Vertex attributes																									
	SQ_API SQUE_VertAttrib* AddAttribute(SQUE_VertAttrib* attribute = NULL);
	SQ_API void EnableAttributesForProgram(const SQUE_Program& program);
	SQ_API void SetLocationsInOrder();
	SQ_API void SetAttributeLocation(const char* name, const int32_t location);

	// Getters																															
	SQ_API uint16_t GetVertSize() const;
	SQ_API uint16_t GetAttribSize(const char* name) const;

	// CleanUp
	SQ_API void CleanUp();

private:
	// Private Variables
	std::vector<SQUE_VertAttrib*> attributes;

	// Private Usage Functions
	void SetOffsetsInOrder();

} SQUE_Mesh;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURES ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																																		
typedef struct SQUE_TexAttrib
{
	// Constructors / Destructor
	SQ_API SQUE_TexAttrib();
	SQ_API SQUE_TexAttrib(int32_t parameter_id, int32_t var_type, void* data);
	// Variables
	int32_t id;
	int32_t var_type;
	void* data;

	// Usage Functions																															

} SQUE_TexAttrib;

typedef struct SQUE_Texture2D
{
	// Constructors / Destructor
	SQ_API SQUE_Texture2D();
	SQ_API SQUE_Texture2D(int32_t format, int32_t var_type);
	SQ_API ~SQUE_Texture2D();

	// Variables
	uint32_t id;
	int32_t format;
	int32_t var_type;
	uint16_t var_size;
	int32_t w, h;
	int32_t channel_num;
	void* pixels;
	std::vector<SQUE_TexAttrib*> attributes;

	// Usage Functions	
	SQ_API void SetParameter(SQUE_TexAttrib* tex_attrib = NULL);
	SQ_API void ApplyParameters();

	SQ_API void CleanUp();

} SQUE_Texture2D;


// SQUE_Texture3D??...																													
typedef struct SQUE_Texture3D
{
	uint32_t id = 0;
	int32_t format = 0;
	int w, h;
	uchar** pixels = nullptr;

	// Cp[y from SQUE_Texture2D																												
} SQUE_Texture3D;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADERS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct SQUE_Shader
{
// Constructor / Destructor
	SQ_API SQUE_Shader();
	SQ_API SQUE_Shader(int32_t type, uint16_t strs, const char** data);

	SQ_API ~SQUE_Shader();
// Variables
	int32_t id = 0;
	int32_t type;
	uint16_t lines = 0;
	const char** source;

// Usage Functions
	SQ_API void Compile();
} SQUE_Shader;

typedef struct SQUE_Uniform
{
// Constructors / Destructors
	SQ_API SQUE_Uniform();
	SQ_API SQUE_Uniform(const char*);
	SQ_API ~SQUE_Uniform();

// Variables
	int32_t id = INT32_MAX;
	const char* name = "";
} SQUE_Uniform;

// Usage Functions
SQ_API void SetBool(const SQUE_Program& prog, const char* name, bool value);
SQ_API void SetInt(const SQUE_Program& prog, const char* name, int value);
SQ_API void SetFloat(const SQUE_Program& prog, const char* name, float value);
SQ_API void SetFloat2(const SQUE_Program& prog, const char* name, glm::vec2 value);
SQ_API void SetFloat3(const SQUE_Program& prog, const char* name, glm::vec3 value);
SQ_API void SetFloat4(const SQUE_Program& prog, const char* name, glm::vec4 value);
// ... add a matrix/array passer...																									
SQ_API void SetMatrix4(const SQUE_Program& prog, const char* name, const float* matrix, uint16_t number_of_matrices = 1, bool transpose = false);

#include <vector>																														
typedef struct SQUE_Program
{

// Constructors / Destructors
	SQ_API SQUE_Program();
	
	SQ_API ~SQUE_Program();
// Variables
	int32_t id = 0;

// Usage Functions																		
	SQ_API SQUE_Shader* AttachShader(SQUE_Shader* sque_shader);
	SQ_API void FreeShadersFromGPU(); // Not required, but saves space after linking

	SQ_API void DeclareUniform(const char*);
	SQ_API int32_t GetUniformLocation(const char* name) const;

	SQ_API void CleanUp();
	
private:
	SQUE_Shader* vertex_s;
	SQUE_Shader* fragment_s;
	// Compute, Tesselation, Geometry,...
	std::vector<SQUE_Uniform*> uniforms;

} SQUE_Program;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ColorRGBA {																												
	ColorRGBA(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}													
	float r, g, b, a;																													
} ColorRGBA;																															
																																		
typedef struct SQUE_RenderState																											
{																																		
	bool backed_up = false;	
	// Have to change into a vector similar to attributes...
	int32_t bound_texture, active_texture_unit;
	int32_t sampler;
	int32_t program, vertex_array_buffer, element_array_buffer;																			
	int32_t attribute_object;																												
	int32_t blend_equation_rgb, blend_equation_alpha;																						
																																		
	bool blend_func_separate = false;																									
	int32_t blend_func_src_rgb, blend_func_dst_rgb;																						
	int32_t blend_func_src_alpha, blend_func_dst_alpha;																					
																																		
	int32_t viewport[4];																														
	int32_t scissor_box[4];																													
	int32_t polygon_mode[2];																													
	bool blend, cull_faces, depth_test, scissor_test;																					
																																		
	SQ_API void SetUp();																												
	SQ_API void BackUp();																												
} SQUE_RenderState;																														
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API bool SQUE_RENDER_Init();																											
SQ_API void SQUE_RENDER_Close();																											
																																		
SQ_API void SQUE_RENDER_ChangeFramebufferSize(int32_t width, int32_t height);		
SQ_API void SQUE_RENDER_GetFramebufferSize(int32_t* width, int32_t* height);
SQ_API void SQUE_RENDER_Clear(const ColorRGBA& color_rgba, int clear_flags = NULL);
SQ_API const char* SQUE_RENDER_GetGLSLVer();																								
																																		
// Function Passthrough/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_RENDER_Scissor(int x, int y, int w, int h);		
// Data Management /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_GenerateMeshBuffer(SQUE_Mesh* mesh);
SQ_API void SQUE_BindMeshBuffer(const SQUE_Mesh& mesh);
SQ_API void SQUE_SendMeshToGPU(const SQUE_Mesh& mesh);
// Vertex Attribute Management /////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_EnableProgramAttribute(const SQUE_Program& prog, SQUE_VertAttrib* attr);
SQ_API void SQUE_SendAttributeToGPU(const SQUE_VertAttrib& attr);
void SQUE_EnableBufferAttribute(int32_t location, uint16_t vert_size, SQUE_VertAttrib* attr);

// Texture Attribute Management ////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_SetTextureParameters(const SQUE_Texture2D& tex, const SQUE_TexAttrib& attr);
// Overload for Texture3D or at some point try to go back to C?

// Texture Management //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_GenTextureData(SQUE_Texture2D* tex);
SQ_API void SQUE_BindTexture(const SQUE_Texture2D& tex);
SQ_API void SQUE_SetActiveTextureUnit(int32_t unit);
SQ_API void SQUE_SendTextureToGPU(const SQUE_Texture2D& tex, int32_t mipmap_level = 0);

SQ_API void SQUE_RENDER_ActiveTexture(int32_t texture_id);
SQ_API void SQUE_RENDER_BindExternalTexture(int tex_type, uint32_t id);
SQ_API void SQUE_RENDER_BindSampler(int32_t texture_locator, int32_t sampler_id);

// Shader Management ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Shader Program Management ///////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_RENDER_CreateProgram(SQUE_Program* prog);
SQ_API void SQUE_RENDER_LinkProgram(const SQUE_Program& prog);
SQ_API void SQUE_RENDER_UseProgram(const SQUE_Program& prog);

// RENDERING ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_RENDER_DrawIndices(const SQUE_Mesh& mesh, int32_t offset_indices = 0, int32_t count = -1);
SQ_API void SQUE_RENDER_DrawVertices(const SQUE_Mesh& mesh, int32_t count = 0);

// Debugging
SQ_API void SQUE_SHADER_CheckCompileLog(const SQUE_Shader& sque_shader);
SQ_API void SQUE_RENDER_CheckProgramLog(const SQUE_Program& sque_program);
SQ_API void CheckForRenderErrors(const char* file, int line);																			
#define SQUE_CHECK_RENDER_ERRORS() SQ_MACRO CheckForRenderErrors(__FILE__, __LINE__)														
			
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILESYSTEM //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  i don't like having to inlcude std::stirng..........
#include <string>

enum SQUE_Assets
{
	
};

typedef struct SQUE_Dir
{
	SQUE_Dir* parent;
	std::vector<SQUE_Dir*> children;

	const char* name;

	char* native_dir_data;
} SQUE_Dir;

typedef struct SQUE_Asset
{
	uint16_t type;
	int64_t size;
	char* raw_data;
}	SQUE_Asset;

SQ_API std::string SQUE_FS_GetExecPath();
// On Android, all these functions require permissions
SQ_API bool SQUE_FS_CreateDirFullPath(const char* path);
SQ_API bool SQUE_FS_CreateDirRelative(const char* path, int32_t flags = NULL);
SQ_API SQUE_Asset* SQUE_FS_LoadFileRaw(const char* path);
SQ_API bool SQUE_FS_WriteFileRaw(const char* path, char* data);

// Permission safe functions
SQ_API SQUE_Dir* SQUE_FS_CreateBaseDirTree();
SQ_API void SQUE_FS_UpdateTree(SQUE_Dir* root);
SQ_API SQUE_Dir* SQUE_FS_GetDirInTree(SQUE_Dir* root, const char* leaf);
SQ_API SQUE_Asset* SQUE_FS_GetAssetRaw(SQUE_Dir* start_dir, const char* file);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOAD/SAVE ASSETS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQ_API bool SQUE_LOAD_Texture(SQUE_Asset* tex_bytes, SQUE_Texture2D* texture);
SQ_API void SQUE_SAVE_Texture(SQUE_Texture2D* texture);

#endif // _SQUE_LIB_ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////