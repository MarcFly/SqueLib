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


#if defined(Debug) || defined(_DEBUG)
#	include <assert.h>
#	define SQ_ASSERT(_EXPR) assert(_EXPR)
#else
#	define SQ_ASSERT(_EXPR)
#endif

// Includes from Standard Library
// TODO : Eliminate what is not necessary or can be optimized
// The incldues are all here because they interfere with mmgr -> included after so we don't mess with the stl
#include <stdint.h> // i like detailed integer types
#include <cstring> // most modules
#include <string> // filesystem makes use of it for practicality
#include <fstream> // filesystem
#include <list> // char_buffer at input -> make my own
#include <cstdarg> // Logger from declaration																						
#include <cstddef>	// Logger from declaration
#include <mutex> // someday this will be threadsafe...
#include <ctime> // Logger
#include <chrono> // Logger and Timer
#include <algorithm> // Logger
#include <unordered_map> // Logger

// Includes from own libs for organization /////////////////////////////////////////////////////////////////////////////////////////////
#include <sque_remap_macros.h>																											
#include <sque_simple_types.h>
#include <sque_data_structures.h>	
#include <sque_sort.h>




// Initialization / State Control //////////////////////////////////////////////////////////////////////////////////////////////////////
#define SQUE_VERSION_MAJOR 2021																											
#define SQUE_VERSION_MINOR 1																												
#define SQUE_VERSION ((SQUE_VERSION_MAJOR << 16) | SQUE_VERSION_MINOR)																		

SQ_API void SQUE_LIB_Init(const char* app_name, int32_t flags = SQ_INIT_DEFAULTS);																									
SQ_API void SQUE_LIB_Close();																									
SQ_API unsigned int SQUE_LIB_GetVersion();																							
SQ_API int SQUE_LIB_IsCompatibleDLL();		
SQ_API void SQUE_LIB_InitRNG(uint64_t seed = NULL);

SQ_API int SQUE_VarGetSize(int type_macro);
SQ_API void SQUE_ConsolePrint(int lt, const char* log);
SQ_API void SQUE_PrintVargs(SQUE_LogType lt, const char file[], int line, const char* format, ...);
#define SQUE_PRINT(LogType, format,...) SQ_MACRO SQUE_PrintVargs(LogType, __FILE__, __LINE__, format, ##__VA_ARGS__)				
SQ_API uint32_t SQUE_RNG(uint32_t max = (UINT32_MAX-1)); // -1 To allow use of MACRO UINT32_MAX for invalid ids and such


// Permissions /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API int16_t SQUE_AskPermissions(const char* permission_name);

// Callback Setters - Flow Management //////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API VoidFun* SQUE_AddOnResumeCallback(VoidFun* fn);																				
SQ_API VoidFun* SQUE_AddOnGoBackgroundCallback(VoidFun* fn);																			
SQ_API void SQUE_Sleep(uint32_t ms);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGGER //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																																
// Types / Strucs //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOGSIZE 1024

struct SQUE_Log
{
	int type = -1;
	SQUE_LogType lt = LT_INFO;
	char log[LOGSIZE] = { 0 };
};
typedef std::pair<int, SQUE_Log> PairLOG;

// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API bool SQUE_LOGGER_Init(bool dumpdata);
SQ_API void SQUE_LOGGER_Close();
SQ_API void SQUE_LOGGER_DumpData();

// Function Usage //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																																												
SQ_API void SQUE_LOGGER_Log(SQUE_LogType lt, const char file[], int line, const char* format, ...);																		
#define SQUE_LOG(LogType,format,...) SQ_MACRO SQUE_LOGGER_Log(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)								

const std::unordered_map<int, std::string*>& SQUE_LOGGER_GetKeys();
const sque_vec<PairLOG>& SQUE_LOGGER_GetLogs();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TIMER ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																																															
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SQUE_Timer																												
{		
public:																																
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
																																		
	bool is_stopped;																										
	bool is_active;																										
};																															
																																		
																																		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Flag setting and unsettign using FLAG MACROS in sque_simple_types.h 																																	
enum SQUE_WindowFlags
{
	SQUE_WINDOW_MOUSE_IN = BITSET1,
	SQUE_WINDOW_TO_MAXIMIZE = BITSET4,
	SQUE_WINDOW_TO_CLOSE = BITSET5,
	SQUE_WINDOW_DEBUG = BITSET6,
};																																	
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_DISPLAY_Init(const char* title = NULL, const uint32_t width = 0, const uint32_t height = 0);
SQ_API void SQUE_DISPLAY_Close();		
SQ_API void SQUE_DISPLAY_NextWindow_WindowHints(int32_t* options, const int32_t size);
SQ_API void SQUE_DISPLAY_NextWindow_ContextHints(int32_t* options, const int32_t size);
SQ_API void SQUE_DISPLAY_NextWindow_BufferHints(int32_t* options, const int32_t size);
SQ_API void SQUE_DISPLAY_NextWindow_Title(const char* title);
SQ_API void SQUE_DISPLAY_NextWindow_Size(const uint32_t width, const uint32_t height);
SQ_API uint16_t SQUE_DISPLAY_OpenWindow(const char* title = NULL, const uint32_t width = 0, const uint32_t height = 0, const uint16_t monitor = 0);
																														
// Setters
SQ_API void SQUE_DISPLAY_SetVSYNC(const int16_t vsync_val);
SQ_API void SQUE_DISPLAY_SetWindowClose(const uint16_t window = 0);																																									
SQ_API uint16_t SQUE_DISPLAY_CloseWindow(const uint16_t window = 0);																																											
SQ_API void SQUE_DISPLAY_ResizeWindow(const uint16_t w, const uint16_t h,const uint16_t window = 0);
SQ_API void SQUE_DISPLAY_UpdateNativeWindowSize(const uint16_t window = 0);
SQ_API void SQUE_DISPLAY_SetWindowIcon(const int32_t width, const int32_t height, void* pixels, const uint16_t window = 0);
SQ_API void SQUE_DISPLAY_SetMouseMode(const int32_t value, const uint16_t window = 0);

// Getters
SQ_API bool SQUE_DISPLAY_ShouldWindowClose(const uint16_t window = 0);
SQ_API uint16_t SQUE_DISPLAY_GetAmountWindows();
SQ_API void SQUE_DISPLAY_GetWindowPos(int32_t* x, int32_t* y, const uint16_t window = 0);
SQ_API void SQUE_DISPLAY_GetWindowSize(int32_t* w, int32_t* h, const uint16_t window = 0);
SQ_API void SQUE_DISPLAY_GetViewportSize(int32_t* w, int32_t* h, const uint16_t window = 0);
SQ_API void* SQUE_DISPLAY_GetPlatformWindowHandle(const uint16_t window = 0);
SQ_API int32_t SQUE_DISPLAY_GetDPIDensity(const uint16_t window = 0);
SQ_API void SQUE_DISPLAY_GetMainDisplaySize(uint16_t* w, uint16_t* h);

// TODO: Android Notifications + GLFW Attention Request

// Controlling Contexts ////////////////////////////////////////////////////////////////////////////////////////////////////////////////																										
SQ_API void SQUE_DISPLAY_SwapBuffer(const uint16_t window = 0);
SQ_API void SQUE_DISPLAY_SwapAllBuffers();																								
SQ_API void SQUE_DISPLAY_MakeContextMain(const uint16_t window = 0);
SQ_API ResizeCallback* SQUE_DISPLAY_SetViewportResizeCallback(ResizeCallback* viewport_cb);
SQ_API ViewportGetSizeCallback* SQUE_DISPLAY_SetViewportGetSizeCallback(ViewportGetSizeCallback* viewport_size_cb);
SQ_API HandleDropFileFun* SQUE_DISPLAY_SetDropFileCallback(HandleDropFileFun* drop_file_cb);

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
	SQUE_KEY_SPACE = 32, SQUE_KEY_SINGLE_QUOTE = 39, SQUE_KEY_ASTERISK = 42, SQUE_KEY_PLUS = 43,
	SQUE_KEY_COMMA = 44, SQUE_KEY_DASH = 45, SQUE_KEY_DOT = 46, SQUE_KEY_FORW_SLASH = 47,
	SQUE_KEY_ZERO_0 = 48, SQUE_KEY_ONE_1 = 49, SQUE_KEY_TWO_2 = 50, SQUE_KEY_THREE_3 = 51,
	SQUE_KEY_FOUR_4 = 52, SQUE_KEY_FIVE_5 = 53, SQUE_KEY_SIX_6 = 54, SQUE_KEY_SEVEN_7 = 55,
	SQUE_KEY_EIGHT_8 = 56, SQUE_KEY_NINE_9 = 57, SQUE_KEY_COLON = 58, SQUE_KEY_SEMI_COLON = 59,
	SQUE_KEY_EQUALS = 61, SQUE_KEY_GREATER_THAN = 62, SQUE_KEY_END_QUESTION = 63, SQUE_KEY_AT = 64,
	SQUE_KEY_UPPER_A = 65, SQUE_KEY_UPPER_B = 66, SQUE_KEY_UPPER_C = 67, SQUE_KEY_UPPER_D = 68,
	SQUE_KEY_UPPER_E = 69, SQUE_KEY_UPPER_F = 70, SQUE_KEY_UPPER_G = 71, SQUE_KEY_UPPER_H = 72,
	SQUE_KEY_UPPER_I = 73, SQUE_KEY_UPPER_J = 74, SQUE_KEY_UPPER_K = 75, SQUE_KEY_UPPER_L = 76,
	SQUE_KEY_UPPER_M = 77, SQUE_KEY_UPPER_N = 78, SQUE_KEY_UPPER_O = 79, SQUE_KEY_UPPER_P = 80,
	SQUE_KEY_UPPER_Q = 81, SQUE_KEY_UPPER_R = 82, SQUE_KEY_UPPER_S = 83, SQUE_KEY_UPPER_T = 84,
	SQUE_KEY_UPPER_U = 85, SQUE_KEY_UPPER_V = 86, SQUE_KEY_UPPER_W = 87, SQUE_KEY_UPPER_X = 88,
	SQUE_KEY_UPPER_Y = 89, SQUE_KEY_UPPER_Z = 90, SQUE_KEY_OPEN_BRACKET = 91, SQUE_KEY_BACK_SLASH = 92,
	SQUE_KEY_CLOSE_BRACKET = 93, SQUE_KEY_OPEN_ACCENT = 96,

	// 54 keys here																														

	// MAIN USAGE CHARACTERS																											
	SQUE_KEY_WORLD_1 = 161, SQUE_KEY_WORLD_2 = 162, SQUE_KEY_ESCAPE = 256, SQUE_KEY_ENTER = 257,
	SQUE_KEY_TAB = 258, SQUE_KEY_BACKSPACE = 259, SQUE_KEY_INSERT = 260, SQUE_KEY_DELETE = 261,
	SQUE_KEY_ARROW_RIGHT = 262, SQUE_KEY_ARROW_LEFT = 263, SQUE_KEY_ARROW_DOWN = 264, SQUE_KEY_ARROW_UP = 265,
	SQUE_KEY_PAGE_UP = 266, SQUE_KEY_PAGE_DOWN = 267, SQUE_KEY_HOME = 268, SQUE_KEY_END = 269,
	SQUE_KEY_CAPS_LOCK = 281, SQUE_KEY_SCROLL_LOCK = 281, SQUE_KEY_NUM_LOCK = 282, SQUE_KEY_PRINT_SCREEN = 283,
	SQUE_KEY_PAUSE = 284, SQUE_KEY_F1 = 290, SQUE_KEY_F2 = 291, SQUE_KEY_F3 = 292,
	SQUE_KEY_F4 = 293, SQUE_KEY_F5 = 294, SQUE_KEY_F6 = 295, SQUE_KEY_F7 = 296,
	SQUE_KEY_F8 = 297, SQUE_KEY_F9 = 298, SQUE_KEY_F10 = 299, SQUE_KEY_F11 = 300,
	SQUE_KEY_F12 = 301, SQUE_KEY_F13 = 302, SQUE_KEY_F14 = 303, SQUE_KEY_F15 = 304,
	SQUE_KEY_F16 = 305, SQUE_KEY_F17 = 306, SQUE_KEY_F18 = 307, SQUE_KEY_F19 = 308,
	SQUE_KEY_F20 = 309, SQUE_KEY_F21 = 310, SQUE_KEY_F22 = 311, SQUE_KEY_F23 = 312,
	SQUE_KEY_F24 = 313, SQUE_KEY_F25 = 314, SQUE_KEY_KEYPAD_1 = 320, SQUE_KEY_KEYPAD_2 = 321,
	SQUE_KEY_KEYPAD_3 = 322, SQUE_KEY_KEYPAD_4 = 323, SQUE_KEY_KEYPAD_5 = 324, SQUE_KEY_KEYPAD_6 = 325,
	SQUE_KEY_KEYPAD_7 = 326, SQUE_KEY_KEYPAD_8 = 327, SQUE_KEY_KEYPAD_9 = 328, SQUE_KEY_KEYPAD_DECIMAL = 329,
	SQUE_KEY_KEYPAD_DIVIDE = 331, SQUE_KEY_KEYPAD_MULT = 332, SQUE_KEY_KEYPAD_SUBTRACT = 333, SQUE_KEY_KEYPAD_ADD = 334,
	SQUE_KEY_KEYPAD_ENTER = 335, SQUE_KEY_KEYPAD_EQUAL = 336, SQUE_KEY_LEFT_SHIFT = 340, SQUE_KEY_LEFT_CTRL = 341,
	SQUE_KEY_LEFT_ALT = 342, SQUE_KEY_LEFT_SUPER = 343, SQUE_KEY_RIGHT_SHIFT = 344, SQUE_KEY_RIGHT_CTRL = 345,
	SQUE_KEY_RIGHT_ALT = 347, SQUE_KEY_RIGHT_SUPER = 348,
	// 66 keys here																														
	// EXTENDED ASCII CHARACTERS																										

	// END																																
	SQUE_KEY_MAX
};
#define NUM_KEYS 121																													
#define MAX_KEYS 512																													
#define MAX_MOUSE_BUTTONS 16																											
enum SQUE_MOUSE_BUTTONS
{
	SQUE_MOUSE_LEFT = 0,	SQUE_MOUSE_RIGHT,	SQUE_MOUSE_CENTER,	SQUE_MOUSE_1,
	SQUE_MOUSE_2,	SQUE_MOUSE_3, SQUE_MOUSE_4, SQUE_MOUSE_5,
	SQUE_MOUSE_6,	SQUE_MOUSE_7, SQUE_MOUSE_8, SQUE_MOUSE_9,
	SQUE_MOUSE_10,	SQUE_MOUSE_11, SQUE_MOUSE_12, SQUE_MOUSE_13,
};
#define MAX_POINTERS 10																													
#define GESTURE_REFRESH 10 // in ms																										
#define MAX_MIDPOINTS 10																												
		
// TODO: merge Mouse and Touch controls somehow
// Find a way to think of pointer intro mouse but not actually
// Have to call thing like a traditional mouse

enum SQUE_INPUT_Actions																													
{																																		
	SQUE_ACTION_UNKNOWN = -1,																											
	// Button States																													
	SQUE_ACTION_RELEASE,																													
	SQUE_ACTION_PRESS,																													
	SQUE_ACTION_REPEAT,																													
																																		
	// Single Touch Controls
	SQUE_ACTION_TAP,																																																									
	SQUE_ACTION_SWIPE_UP,																												
	SQUE_ACTION_SWIPE_DOWN,																												
	SQUE_ACTION_SWIPE_LEFT,																												
	SQUE_ACTION_SWIPE_RIGHT,																												
	// Gameplay Actions																													
																																		
	// 																																	
	SQUE_ACTION_MAX																														
};																																		
																																		
void DebugKey(const int32_t code, const int32_t state);																									
struct SQUE_Key																													
{																																		
	//int key;																															
	int prev_state = -1;																												
	int state = -1;																														
	KeyCallback* callback = DebugKey;																									
};																																
																																		
																																		
void DebugMouseFloatCallback(float x, float y);																							
																																		
// Touch Display Oriented - But will implement mouse based gestures and callback based key gestures	////////////////////////////////////
struct SQUE_Gesture																												
{																																		
	SQUE_Timer timer;																													
																																		
	float start_x = INT32_MAX, start_y = INT32_MAX;																												
	float midpoints[MAX_MIDPOINTS][2];																									
	float end_x = INT32_MAX, end_y = INT32_MAX;																													
																																		
	float refresh_bucket = 0;																												
};																															
																																		
struct SQUE_Pointer																												
{																																		
	bool active = false;																												
	int32_t id;	
																							
	float x = -1, y = -1;																									
	MouseFloatCallback* pos_callback = DebugMouseFloatCallback;
};																															
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_INPUT_Init();
SQ_API void SQUE_INPUT_InitForWindow(const uint16_t window);
SQ_API bool SQUE_INPUT_Close();																											
SQ_API void SQUE_INPUT_Process(const uint16_t window);																																											
															
// Usage / Utilities ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_INPUT_DisplaySoftwareKeyboard(const bool show);
SQ_API SQUE_INPUT_Actions SQUE_INPUT_DetectGesture(const SQUE_Gesture& g);
SQ_API SQUE_INPUT_Actions SQUE_INPUT_EvalGesture();

// Setters /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_INPUT_SetMousePos(const float x, const float y);																						
SQ_API void SQUE_INPUT_SetPointerActive(const uint16_t pointer, const bool active);

// Getters /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API SQUE_INPUT_Actions SQUE_INPUT_GetKey(const SQUE_KeyboardKeys key);
SQ_API SQUE_INPUT_Actions SQUE_INPUT_GetMouseButton(const uint32_t button);
SQ_API void SQUE_INPUT_GetPointerAvgPos(float* x, float* y, const uint16_t points = 1);
SQ_API bool SQUE_INPUT_GetPointerPos(float* x, float* y, const uint16_t pointer = 0);
SQ_API void SQUE_INPUT_GetScroll(float* v = NULL, float* h = NULL);
SQ_API int SQUE_INPUT_GetCharFromBuffer();

// Callback Setters ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API KeyCallback* SQUE_INPUT_SetKeyCallback(KeyCallback* sque_key_fn);																		
SQ_API MouseFloatCallback* SQUE_INPUT_SetPointerPosCallback(MouseFloatCallback* position, const uint16_t pointer);
SQ_API MouseFloatCallback* SQUE_INPUT_SetScrollCallback(MouseFloatCallback* scroll);
SQ_API KeyCallback* SQUE_INPUT_SetMouseButtonCallback(const int button, KeyCallback* key_callback);												
	

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SQUE_VertAttrib
{
public:
	// Constructors / Destructors
	SQ_API SQUE_VertAttrib();
	SQ_API SQUE_VertAttrib(const char* name_, const int32_t var_type, const bool normalize, const uint16_t num_components);
	SQ_API ~SQUE_VertAttrib();

	// Variables
	int32_t id = 0;
	int32_t var_type = SQUE_FLOAT;
	uint16_t num_comp = 0;
	bool normalize = false;
	uint16_t var_size = 4;
	uint16_t offset = 0;

	char name[89] = "";
};

// Maybe Swap to a custom array handler for specific sizes																				
// I want to have afast search on less than 100 objects, probably a full array is good enough	
class SQUE_Mesh
{
public:
// Constructors / Destructors
	SQ_API SQUE_Mesh();
	SQ_API ~SQUE_Mesh();

// Variables
	int32_t draw_config;
	int32_t draw_mode = SQUE_STATIC_DRAW;
	uint32_t attribute_object = 0;

	// Vertex Data																														
	uint32_t vert_id = 0;
	uint32_t num_verts = 0;
	
	// Vertex Layout
	// vector<Attributos>

	// Index Data																											
	uint32_t index_id = 0;
	uint16_t num_index = 0;
	uint32_t index_var = SQUE_UINT; // Default 4 because generally used uint, but ImGui Uses 2 Byte indices								
	uint16_t index_var_size = 0;

	sque_vec<SQUE_VertAttrib> attributes;
	uint16_t vertex_size = 0;
};
// Usage Functions
SQ_API void SQUE_MESH_SetDrawConfig(SQUE_Mesh* mesh, const int32_t draw_config, const int32_t draw_mode);
SQ_API void SQUE_MESH_SetDataConfig(SQUE_Mesh* mesh, const uint32_t num_verts, const uint32_t num_index_ = 0, const uint32_t index_var_ = SQUE_UINT);
SQ_API void SQUE_MESH_AddAttribute(SQUE_Mesh* mesh, const char* name_, const int32_t var_type, const bool normalize, const uint16_t num_components);

SQ_API uint16_t SQUE_MESH_CalcVertSize(SQUE_Mesh* mesh);
SQ_API void SQUE_MESH_InterleaveOffsets(SQUE_Mesh* mesh); // Vertex Format is: V1(Pos,Normal,UV,...), V2(...)...
SQ_API void SQUE_MESH_BlockOffsets(SQUE_Mesh* mesh); // Vertex Format is: Positions(P1,P2,...), Normals(N1, N2,...), UV(...),...
SQ_API uint16_t SQUE_MESH_GetAttribSize(const SQUE_Mesh& mesh, const char* name);

SQ_API void SQUE_MESH_SetLocations(SQUE_Mesh* mesh);

// Data Management /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_MESH_GenBufferIDs(const uint32_t num, uint32_t* ids);
SQ_API void SQUE_MESH_GenAttributeObjects(const uint32_t num, uint32_t* attribute_objects);
SQ_API void SQUE_MESH_GenBuffer(SQUE_Mesh* mesh);
SQ_API void SQUE_MESH_BindBuffer(const SQUE_Mesh& mesh);
SQ_API void SQUE_MESH_BindVertices(const uint32_t vert_id);
SQ_API void SQUE_MESH_BindIndices(const uint32_t index_id);
SQ_API void SQUE_MESH_BindAttributeObject(const uint32_t attribute_object);
SQ_API void SQUE_MESH_SendToGPU(const SQUE_Mesh& mesh, void* vert_data = NULL, void* index_data = NULL);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURES ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																																		
class SQUE_TexAttrib
{
public:
	SQ_API SQUE_TexAttrib();
	SQ_API SQUE_TexAttrib(const char* name, const int32_t attrib_id, const int32_t value);
	SQ_API SQUE_TexAttrib(const char* name, const int32_t attrib_id, const float value);

	char name[32] = "";
	uint32_t type = UINT32_MAX;
	int32_t id = UINT32_MAX;
	void* data = NULL;	
};

struct SQUE_Texture
{
	uint32_t id = 0;
	int32_t dim_format = -1;
	int32_t use_format = -1;
	int32_t data_format = -1;
	int32_t var_type = SQUE_UBYTE;
	uint16_t var_size = 1;

	int32_t w = 0, h = 0;
	int32_t channel_num = 4;

	sque_vec<SQUE_TexAttrib> attributes;
};

SQ_API void SQUE_TEXTURE_SetFormat(SQUE_Texture* texture, const int32_t dimentions_format, const int32_t use_f, const int32_t data_f, const int32_t var_type);
SQ_API void SQUE_TEXTURE_SetDimentions(SQUE_Texture* texture, const int32_t width, const int32_t height, const int32_t num_channels);
SQ_API void SQUE_TEXTURE_AddAttribute(SQUE_Texture* texture, const char* name, int32_t attrib_id, int32_t value);
SQ_API void SQUE_TEXTURE_AddAttribute(SQUE_Texture* texture, const char* name, int32_t attrib_id, float value);

SQ_API void SQUE_TEXTURE_GenBufferIDs(const uint32_t num, uint32_t* tex_ids);
SQ_API void SQUE_TEXTURE_GenMipmaps(const uint32_t texture_type);
SQ_API void SQUE_TEXTURE_Bind(const uint32_t texture_id, const int32_t texture_dims);
SQ_API void SQUE_TEXTURE_ApplyAttributes(const SQUE_Texture& tex);
SQ_API void SQUE_TEXTURE_SetActiveUnit(const int32_t unit);
SQ_API void SQUE_TEXTURE_SendAs2DToGPU(const SQUE_Texture& tex, void* pixels, const int32_t mipmap_level = 0);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADERS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SQUE_Uniform
{
	int32_t id = -1;
	uint32_t type = -1;
	int32_t var_size = 0;
	char name[52] = "";
};

struct SQUE_Shader
{
	int32_t id = 0;
	int32_t type = NULL;
};

SQ_API void SQUE_SHADERS_GenerateID(SQUE_Shader* shader, const int32_t shader_type);
SQ_API void SQUE_SHADERS_SetSource(const int32_t shader_id, const char* source_);
SQ_API void SQUE_SHADERS_Compile(const int32_t shader_id);
SQ_API void SQUE_SHADERS_FreeFromGPU(const int32_t shader_id);

struct SQUE_Program
{
	// Variables
	uint32_t id = 0;
	uint32_t shaders[2];
	sque_vec<SQUE_Uniform> uniforms;
	//sque_vec<SQUE_VertAttrib> attributes;
	// IDs of the shaders // Order by execution stage
	// 0 Vertex -> 1 Fragment 
	// Geometry, Tesselation and Compute will be added later if I want to and have time to
	// They require more setup and steps
};

SQ_API void SQUE_PROGRAM_AttachShader(SQUE_Program* program, const SQUE_Shader& shader);
SQ_API void SQUE_PROGRAM_FreeShadersFromGPU(int32_t shaders[]); // Not required, but saves space after linking
SQ_API void SQUE_PROGRAM_CacheUniforms(SQUE_Program* program);

SQ_API void SQUE_PROGRAM_GenerateID(uint32_t* program_id);
SQ_API void SQUE_PROGRAM_Link(const uint32_t program_id);
SQ_API void SQUE_PROGRAM_Use(const uint32_t program_id);

// Usage Functions
SQ_API int32_t SQUE_PROGRAM_GetUniformID(const SQUE_Program& program, const char* uniform_name);

SQ_API void SetBool(const int32_t uniform_id, const bool value);
SQ_API void SetInt(const int32_t uniform_id, const int32_t value);
SQ_API void SetFloat(const int32_t uniform_id, const float value);
SQ_API void SetFloat2(const int32_t uniform_id, const float value[2]);
SQ_API void SetFloat3(const int32_t uniform_id, const float value[3]);
SQ_API void SetFloat4(const int32_t uniform_id, const float value[4]);
// ... add a matrix/array passer...																									
SQ_API void SetMatrix4(const int32_t uniform_id, const float* matrix, const uint16_t number_of_matrices = 1, const bool transpose = false);



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ColorRGBA 
{																												
public:
	SQ_API ColorRGBA();
	SQ_API ColorRGBA(float _r, float _g, float _b, float _a);													
	SQ_API ~ColorRGBA();

	float r, g, b, a;																													
};																															
																																		
class SQUE_RenderState																											
{	
public:
	SQ_API SQUE_RenderState();
	SQ_API ~SQUE_RenderState();
	
																													
	int32_t blend_equation_rgb, blend_equation_alpha;																						
																																		
	bool blend_func_separate = false;																									
	int32_t blend_func_src_rgb, blend_func_dst_rgb;																						
	int32_t blend_func_src_alpha, blend_func_dst_alpha;																					
																																																																																										
	int32_t polygon_mode[2];																													
	bool blend, cull_faces, depth_test, scissor_test;																					
						
	int32_t draw_framebuffer, read_framebuffer;

	SQ_API void SetUp();																												
	SQ_API void BackUp();

	private:
	bool backed_up = false;	
};																														
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API bool SQUE_RENDER_Init();																											
SQ_API void SQUE_RENDER_Close();																											
																																		
SQ_API void SQUE_RENDER_ChangeFramebufferSize(const int32_t width, const int32_t height);		
SQ_API void SQUE_RENDER_GetFramebufferSize(int32_t* width, int32_t* height);
SQ_API void SQUE_RENDER_Clear(const ColorRGBA& color_rgba, const int clear_flags = SQUE_COLOR_BIT);
SQ_API const char* SQUE_RENDER_GetShaderHeader();																								
																																		
// Function Passthrough/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_RENDER_Scissor(const int x, const int y, const int w, const int h);
SQ_API void SQUE_RENDER_GetViewport(int32_t* x, int32_t* y, int32_t* w, int32_t* h);
SQ_API void SQUE_RENDER_GetIntV(const int32_t value_id, int32_t* value);
SQ_API void SQUE_RENDER_SetViewport(const int x, const int y, const int w, const int h);
SQ_API void SQUE_RENDER_SetPolyMode(const int32_t faces, const int32_t mode);	
SQ_API void SQUE_RENDER_BindSampler(const int32_t texture_locator, const int32_t sampler_id);

// Vertex Attribute Management /////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_PROGRAM_EnableAttribute(const SQUE_Program& prog, const uint16_t vert_size, SQUE_VertAttrib* attr);
SQ_API void SQUE_MESH_EnableAttribute(const uint16_t vert_size, const SQUE_VertAttrib& attr);

// RENDERING ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SQ_API void SQUE_RENDER_DrawIndices(const SQUE_Mesh& mesh, const int32_t offset_indices = 0, const int32_t count = -1);
SQ_API void SQUE_RENDER_DrawVertices(const SQUE_Mesh& mesh, const int32_t count = 0);

// Debugging
SQ_API void SQUE_SHADERS_CheckCompileLog(const int32_t shader_id);
SQ_API void SQUE_PROGRAM_CheckLinkLog(const uint32_t program_id);
SQ_API void CheckForRenderErrors(const char* file, const int line);
SQ_API void InitGLDebug();
#define SQUE_CHECK_RENDER_ERRORS() SQ_MACRO CheckForRenderErrors(__FILE__, __LINE__)														

// FRAMEBUFFER MANAGEMENT //////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SQUE_Framebuffer
{
	uint32_t id;
	int32_t type; // Read, Write, Default

	uint32_t width, height;

	uint32_t depth_buffer_id;
	int32_t depth_type;

	uint32_t stencil_buffer_id;
	int32_t stencil_type;

	sque_vec<SQUE_Texture> textures;
};

SQ_API void SQUE_FRAMEBUFFER_GenerateID(uint32_t* framebuffer_id);
SQ_API void SQUE_FRAMEBUFFER_Bind(const int32_t type, const uint32_t id);
SQ_API void SQUE_FRAMEBUFFER_GenRenderTypeID(uint32_t* renderbuffer_id);
SQ_API void SQUE_FRAMEBUFFER_BindRenderType(const uint32_t renderbuffer_id);
SQ_API void SQUE_FRAMEBUFFER_SetRenderTypeInfo(const uint32_t type, const uint32_t width, const uint32_t height);
SQ_API void SQUE_FRAMEBUFFER_AttachRenderType(const uint32_t attachment_type, const uint32_t attachment_id);
SQ_API void SQUE_FRAMEBUFFER_AttachTexture(const uint32_t dest_attachment, const uint32_t texture_id, const uint32_t mipmap_level = 0);
SQ_API void SQUE_FRAMEBUFFER_SetDrawBuffers(const uint32_t* attachments, const uint32_t size = 1);
SQ_API uint32_t SQUE_FRAMEBUFFER_CheckStatus();


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILESYSTEM //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct SQUE_Dir
{
	uint32_t id = UINT32_MAX;
	char name[64];
	char location[512];

	uint32_t parent_id = -1;
	sque_vec<uint32_t> children_ids;
};

SQUE_Dir SQUE_FS_GenDir(const char* location, const uint32_t parent_id = -1);

struct SQUE_Asset
{
	uint64_t size;
	char* raw_data;
};

struct SQUE_DataPack
{
	SQUE_Asset data;
	SQUE_Asset metadata;
};

typedef uint32_t(HandleNewAssetLocation)(const char* location);
typedef void(ReadWriteAssetFun)(const char* location, SQUE_DataPack* datapack);
typedef void(UnloadAssetFun)(SQUE_DataPack* datapack);

enum AssetStatus
{
	SQ_AS_NEUTRAL = BITSET1,
	SQ_AS_CHANGED = BITSET2,
	SQ_AS_DELETED = BITSET3,
	SQ_AS_LOADED = BITSET4,
	//...
};

struct SQUE_CtrlAsset
{
	// Static Data - Generated or Loaded
	uint32_t id = -1;
	char name[64] = "";
	char location[512] = "";
	uint32_t dir_id = UINT32_MAX;
	uint32_t type = -1; //?
	
	// Runtime Updates
	SQUE_Timer unused_timer;
	uint32_t current_users = 0;
	uint8_t status_flags = NULL; // refer to AssetStatus
	double last_update = 0;


	// Type Based functions  
	ReadWriteAssetFun* Save;
	ReadWriteAssetFun* Load;
	UnloadAssetFun* Unload;

	// Actual Data
	SQUE_DataPack datapack;
};

SQ_API void SQUE_FS_Init();
SQ_API const char* SQUE_FS_GetExecPath();
// On Android, all these functions require permissions
SQ_API bool SQUE_FS_CreateDirFullPath(const char* path);
SQ_API bool SQUE_FS_CreateDirRelative(const char* path, const int32_t flags = NULL);
SQ_API SQUE_Asset* SQUE_FS_LoadFileRaw(const char* path);
SQ_API bool SQUE_FS_WriteFileRaw(const char* path, char* data, const uint64_t size);

// Permission safe functions
SQ_API SQUE_Dir* SQUE_FS_CreateBaseDirTree();
SQ_API SQUE_Asset* SQUE_FS_LoadAssetRaw(const char* file);
SQ_API const char* SQUE_FS_GetFileName(const char* file);
SQ_API uint32_t SQUE_FS_GetParentDir(const char* path);
// Filewatcher - std::filesystem based, requires c++17
SQ_API void SQUE_FS_GenDirectoryStructure(const char* location, sque_vec<SQUE_Dir>* dirs);

class SQUE_FW_NewAsset
{
public:
	SQ_API SQUE_FW_NewAsset();
	SQ_API ~SQUE_FW_NewAsset();

	double last_update = 0;
	uint32_t str_len = 0;
	char str[512];
};

SQ_API sque_vec<SQUE_FW_NewAsset*> SQUE_FS_CheckDirectoryChanges(const char* path, const sque_vec<SQUE_CtrlAsset*>& assets_in_dir, HandleNewAssetLocation* handle_fun);

// OpenDDL style Serialization
enum class OPENDDL_IDS : uint8_t
{
	b = 0,
	i8, i16, i32, i64,
	u8, u16, u32, u64,
	f16, f32, f64,
	string, ref, type, base64,
	uid, strct,
};
#include "sque_stream.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOAD/SAVE ASSETS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SQ_API bool SQUE_LOAD_Texture(SQUE_Asset* tex_bytes, SQUE_Texture* texture);
SQ_API void SQUE_FREE_Texture(SQUE_Asset* tex_bytes);
//SQ_API void SQUE_SAVE_Texture(SQUE_Texture* texture);

#endif // _SQUE_LIB_ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////