#ifndef _FLY_LIB_																														
#define _FLY_LIB_																														
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CORE ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Export Markers //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
																																		
// Macro Exporter, some macros that wrap functions are a bit stupid in MSC																
#ifdef _MSC_VER																															
#	define FL_MACRO																														
#else 																																	
#	define FL_MACRO FL_API																												
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
#       define GLFW_EXPOSE_NATIVE_WIN32
#	else
#	endif																						
#endif																																	

// Includes from own libs for organization /////////////////////////////////////////////////////////////////////////////////////////////
#include <fly_remap_macros.h>																											
#include <fly_simple_types.h>																											
#include <stdint.h>
// Currently all math with go through glm, I don't like but I don't have an easy drop in replacement
#include<glm.hpp>
// Initialization / State Control //////////////////////////////////////////////////////////////////////////////////////////////////////
#define FL_VERSION_MAJOR 2020																											
#define FL_VERSION_MINOR 1																												
#define FL_VERSION ((FL_VERSION_MAJOR << 16) | FL_VERSION_MINOR)																		

// FLYLIB should not have an init/close
// User should do that with specifics per module really
FL_API void FLYLIB_Init(const char* app_name, int32_t flags);																									
FL_API void FLYLIB_Close();																									
FL_API unsigned int FLYLIB_GetVersion();																							
FL_API int FLYLIB_IsCompatibleDLL();		

FL_API int FLY_VarGetSize(int type_macro);
FL_API void FLY_ConsolePrint(int lt, const char* log);
FL_API void FLY_PrintVargs(FLY_LogType lt, const char file[], int line, const char* format, ...);
#define FLYPRINT(LogType, format,...) FL_MACRO FLY_PrintVargs(LogType, __FILE__, __LINE__, format, ##__VA_ARGS__)					

// Callback Setters - Flow Management //////////////////////////////////////////////////////////////////////////////////////////////////
FL_API VoidFun FLYINPUT_AddOnResumeCallback(VoidFun fn);																				
FL_API VoidFun FLYINPUT_AddOnGoBackgroundCallback(VoidFun fn);																			

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LOGGER //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <cstdarg>																														
#include <cstddef>																														
																																		
// Types / Strucs //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define LOGSIZE 1024																													
																														
typedef struct FLY_Log																													
{																																		
	int type = -1;																														
	FLY_LogType lt = LT_INFO;																											
	char log[LOGSIZE] = {0};																											
} FLY_Log;																																
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
FL_API bool FLYLOGGER_Init(bool dumpdata);																								
FL_API void FLYLOGGER_Close();																											
																																																														
// Function Usage //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLY_ConsolePrint(int lt, const char* log);																																														
FL_API void FLYLOGGER_Log(FLY_LogType lt, const char file[], int line, const char* format, ...);																		
FL_API void FLYLOGGER_DumpData();

// EASY Usage //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define FLYLOG(LogType,format,...) FL_MACRO FLYLOGGER_Log(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)								
																																		
																																		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TIMER ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
#include <atomic>																														
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct FLY_Timer																												
{																																		
	FL_API FLY_Timer();																													
																																		
	FL_API void Start();																												
	FL_API void Stop();																													
	FL_API void Kill();																													
	FL_API bool IsStopped() const;																											
	FL_API bool IsActive() const;																												
	FL_API double ReadMilliSec() const;																										
	FL_API double ReadMicroSec() const;																										
	FL_API double ReadNanoSec() const;																										
																																		
private:																																
	double start_at_ms;																													
	double start_at_ns;																													
	double start_at_us;																													
																																		
	double stop_at_ms;																													
	double stop_at_ns;																													
	double stop_at_us;																													
																																		
	std::atomic<bool> is_stopped;																										
	std::atomic<bool> is_active;																										
} FLY_Timer;																															
																																		
																																		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Flag setting and unsettign using FLAG MACROS in fly_simple_types.h 																																	
enum FLY_WindowFlags
{
	FLYWINDOW_MOUSE_IN = BITSET1,
	FLYWINDOW_TO_MAXIMIZE = BITSET4,
	FLYWINDOW_TO_CLOSE = BITSET5,
	
};																																	
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLYDISPLAY_Init();
FL_API void FLYDISPLAY_Close();																											
FL_API void FLYDISPLAY_SetVSYNC(int16_t vsync_val);																						
FL_API int32_t FLYDISPLAY_GetDPIDensity(uint16_t window = 0);																				
FL_API void FLYDISPLAY_GetMainDisplaySize(uint16_t& w, uint16_t& h);

																																		
// Control Specific Windows ////////////////////////////////////////////////////////////////////////////////////////////////////////////		
// I don't like having user setup a FLY_Window directly
FL_API void FLYDISPLAY_NextWindow_WindowHints(int32_t* options, int32_t size);
FL_API void FLYDISPLAY_NextWindow_ContextHints(int32_t* options, int32_t size);
FL_API void FLYDISPLAY_NextWindow_BufferHints(int32_t* options, int32_t size);

FL_API void FLYDISPLAY_OpenWindow(const char* title, int32_t width=0, int32_t height=0, uint16_t monitor = 0);
FL_API bool FLYDISPLAY_ShouldWindowClose(uint16_t window);

// Setters
FL_API void FLYDISPLAY_SetWindowClose(uint16_t window);																																									
FL_API uint16_t FLYDISPLAY_CloseWindow(uint16_t window);																																											
FL_API void FLYDISPLAY_ResizeWindow(uint16_t window, uint16_t w, uint16_t h);
FL_API void FLYDISPLAY_UpdateNativeWindowSize(uint16_t window);
// Getters
FL_API uint16_t FLYDISPLAY_GetAmountWindows();
FL_API void FLYDISPLAY_GetWindowPos(uint16_t window, int32_t& x, int32_t& y);																	
FL_API void FLYDISPLAY_GetWindowSize(uint16_t window, int32_t* w, int32_t* h);
FL_API void FLYDISPLAY_GetViewportSize(uint16_t window, int32_t* w, int32_t* h);
FL_API void* FLYDISPLAY_GetPlatformWindowHandle(uint16_t window);

// Controlling Contexts ////////////////////////////////////////////////////////////////////////////////////////////////////////////////																										
FL_API void FLYDISPLAY_SwapBuffer(uint16_t window);
FL_API void FLYDISPLAY_SwapAllBuffers();																								
FL_API void FLYDISPLAY_MakeContextMain(uint16_t window);
FL_API ResizeCallback FLYDISPLAY_SetViewportResizeCallback(ResizeCallback viewport_cb);
FL_API ViewportSizeCallback FLYDISPLAY_SetViewportSizeCallback(ViewportSizeCallback viewport_size_cb);
																																		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
																																		
void DebugKey(int32_t code, int32_t state);																									
typedef struct FLY_Key																													
{																																		
	//int key;																															
	int prev_state = -1;																												
	int state = -1;																														
	KeyCallback callback = DebugKey;																									
} FLY_Key;																																
																																		
																																		
void DebugMouseFloatCallback(float x, float y);																							
typedef struct FLY_Mouse																												
{																																		
	float prev_y = INT32_MAX, prev_x = INT32_MAX;																						
	float x = INT32_MAX, y = INT32_MAX;																									
	MouseFloatCallback pos_callback = DebugMouseFloatCallback;																			
																																		
	float prev_scrollx = INT32_MAX, prev_scrolly = INT32_MAX;																			
	float scrollx = INT32_MAX, scrolly = INT32_MAX;																						
	MouseFloatCallback scroll_callback = DebugMouseFloatCallback;																		
																																		
	FLY_Key mbuttons[MAX_MOUSE_BUTTONS];																								
																																		
} FLY_Mouse;																															
																																		
																																		
// Touch Display Oriented - But will implement mouse based gestures and callback based key gestures	////////////////////////////////////
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
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLYINPUT_Init();
FL_API void FLYINPUT_InitForWindow(uint16_t window);
FL_API bool FLYINPUT_Close();																											
FL_API void FLYINPUT_Process(uint16_t window);																							
FL_API void FLYINPUT_DisplaySoftwareKeyboard(bool show);																				
															
// Usage / Utilities ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLYINPUT_DisplaySoftwareKeyboard(bool show);
FL_API FLYINPUT_Actions FLYINPUT_DetectGesture(FLY_Pointer& p);
FL_API FLYINPUT_Actions FLYINPUT_EvalGesture();

// Setters /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLYINPUT_SetMousePos(float x, float y);																						

// Getters /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API FLYINPUT_Actions FLYINPUT_GetMouseButton(int button);
FL_API void FLYINPUT_GetMousePos(float* x, float* y);
FL_API void FLYINPUT_GetMouseWheel(float* v = NULL, float* h = NULL);
FL_API int FLYINPUT_GetCharFromBuffer();

// Callback Setters ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API KeyCallback FLYINPUT_SetKeyCallback(KeyCallback fly_key_fn);																		
FL_API MouseFloatCallback FLYINPUT_SetMousePosCallback(MouseFloatCallback position);													
FL_API MouseFloatCallback FLYINPUT_SetMouseScrollCallback(MouseFloatCallback scroll);													
FL_API KeyCallback FLYINPUT_SetMouseButtonCallbacks(int button, KeyCallback key_callback);												
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESHES //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct FLY_VertAttrib
{
	// Constructors / Destructors
	FL_API FLY_VertAttrib();
	FL_API FLY_VertAttrib(const char* name_, int32_t var_type, bool normalize, uint16_t num_components);

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
	FL_API uint16_t GetSize() const;

} FLY_VertAttrib;

#include <vector> 																														
// Maybe Swap to a custom array handler for specific sizes																				
// I want to hav e afast search on less than 100 objects, probably a full array is good enough											
struct FLY_Program;
typedef struct FLY_Mesh
{
// Constructors / Destructors
	FL_API FLY_Mesh();
	FL_API FLY_Mesh(int32_t draw_config, int32_t draw_mode, int32_t index_var);

	FL_API ~FLY_Mesh();

// Variables
	int32_t draw_config;
	int32_t draw_mode = FLY_STATIC_DRAW;
	uint32_t attribute_object = 0;

	// Vertex Data																														
	uint32_t vert_id = 0;
	uint32_t num_verts = 0;
	void* verts = NULL;
	// Indices for the buffer																											
	uint32_t index_id = 0;
	uint16_t num_index = 0;
	uint32_t index_var = FLY_UINT; // Default 4 because generally used uint, but ImGui Uses 2 Byte indices								
	uint16_t index_var_size = 0;
	void* indices = nullptr;

// Usage Functions
	// Changing the data dynamically (good for stream/dynamic draw)
	FL_API void ChangeVertData(int32_t num_verts_, void* verts_);
	FL_API void ChangeIndexData(int32_t num_index_, void* indices_);

	// Location and Vertex attributes																									
	FL_API FLY_VertAttrib* AddAttribute(FLY_VertAttrib* attribute = NULL);
	FL_API void EnableAttributesForProgram(const FLY_Program& program);
	FL_API void SetLocationsInOrder();
	FL_API void SetAttributeLocation(const char* name, const int32_t location);

	// Getters																															
	FL_API uint16_t GetVertSize() const;
	FL_API uint16_t GetAttribSize(const char* name) const;

	// CleanUp
	FL_API void CleanUp();

private:
	// Private Variables
	std::vector<FLY_VertAttrib*> attributes;

	// Private Usage Functions
	void SetOffsetsInOrder();

} FLY_Mesh;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURES ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////																																		
typedef struct FLY_TexAttrib
{
	// Constructors / Destructor
	FL_API FLY_TexAttrib();
	FL_API FLY_TexAttrib(int32_t parameter_id, int32_t var_type, void* data);
	// Variables
	int32_t id;
	int32_t var_type;
	void* data;

	// Usage Functions																															

} FLY_TexAttrib;

typedef struct FLY_Texture2D
{
	// Constructors / Destructor
	FL_API FLY_Texture2D();
	FL_API FLY_Texture2D(int32_t format, int32_t var_type);
	FL_API ~FLY_Texture2D();

	// Variables
	uint32_t id;
	int32_t format;
	int32_t var_type;
	uint16_t var_size;
	int32_t w, h;
	int32_t channel_num;
	void* pixels;
	std::vector<FLY_TexAttrib*> attributes;

	// Usage Functions																																																			
	FL_API void SetParameter(FLY_TexAttrib* tex_attrib = NULL);
	FL_API void ApplyParameters();

	FL_API void CleanUp();

} FLY_Texture2D;


// FLY_Texture3D??...																													
typedef struct FLY_Texture3D
{
	uint32_t id = 0;
	int32_t format = 0;
	int w, h;
	uchar** pixels = nullptr;

	// Cp[y from FLY_Texture2D																												
} FLY_Texture3D;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADERS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct FLY_Shader
{
// Constructor / Destructor
	FL_API FLY_Shader();
	FL_API FLY_Shader(int32_t type, uint16_t strs, const char** data);

	FL_API ~FLY_Shader();
// Variables
	int32_t id = 0;
	int32_t type;
	uint16_t lines = 0;
	const char** source;

// Usage Functions
	FL_API void Compile();
} FLY_Shader;

typedef struct FLY_Uniform
{
// Constructors / Destructors
	FL_API FLY_Uniform();
	FL_API FLY_Uniform(const char*);
	FL_API ~FLY_Uniform();

// Variables
	int32_t id = INT32_MAX;
	const char* name = "";
} FLY_Uniform;

// Usage Functions
FL_API void SetBool(const FLY_Program& prog, const char* name, bool value);
FL_API void SetInt(const FLY_Program& prog, const char* name, int value);
FL_API void SetFloat(const FLY_Program& prog, const char* name, float value);
FL_API void SetFloat2(const FLY_Program& prog, const char* name, glm::vec2 value);
FL_API void SetFloat3(const FLY_Program& prog, const char* name, glm::vec3 value);
FL_API void SetFloat4(const FLY_Program& prog, const char* name, glm::vec4 value);
// ... add a matrix/array passer...																									
FL_API void SetMatrix4(const FLY_Program& prog, const char* name, const float* matrix, uint16_t number_of_matrices = 1, bool transpose = false);

#include <vector>																														
typedef struct FLY_Program
{

// Constructors / Destructors
	FL_API FLY_Program();
	
	FL_API ~FLY_Program();
// Variables
	int32_t id = 0;

// Usage Functions																		
	FL_API FLY_Shader* AttachShader(FLY_Shader* fly_shader);
	FL_API void FreeShadersFromGPU(); // Not required, but saves space after linking

	FL_API void DeclareUniform(const char*);
	FL_API int32_t GetUniformLocation(const char* name) const;

	FL_API void CleanUp();
	
private:
	FLY_Shader* vertex_s;
	FLY_Shader* fragment_s;
	// Compute, Tesselation, Geometry,...
	std::vector<FLY_Uniform*> uniforms;

} FLY_Program;



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																																		
// Types / Structs /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct ColorRGBA {																												
	ColorRGBA(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}													
	float r, g, b, a;																													
} ColorRGBA;																															
																																		
typedef struct FLY_RenderState																											
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
																																		
	FL_API void SetUp();																												
	FL_API void BackUp();																												
} FLY_RenderState;																														
																																		
// Initialization / State Management ///////////////////////////////////////////////////////////////////////////////////////////////////
FL_API bool FLYRENDER_Init();																											
FL_API void FLYRENDER_Close();																											
																																		
FL_API void FLYRENDER_ChangeFramebufferSize(int32_t width, int32_t height);		
FL_API void FLYRENDER_GetFramebufferSize(int32_t* width, int32_t* height);
FL_API void FLYRENDER_Clear(const ColorRGBA& color_rgba, int clear_flags = NULL);
FL_API const char* FLYRENDER_GetGLSLVer();																								
																																		
// Function Passthrough/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLYRENDER_Scissor(int x, int y, int w, int h);		
// Data Management /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLY_GenerateMeshBuffer(FLY_Mesh* mesh);
FL_API void FLY_BindMeshBuffer(const FLY_Mesh& mesh);
FL_API void FLY_SendMeshToGPU(const FLY_Mesh& mesh);
// Vertex Attribute Management /////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLY_EnableProgramAttribute(const FLY_Program& prog, FLY_VertAttrib* attr);
FL_API void FLY_SendAttributeToGPU(const FLY_VertAttrib& attr);
void FLY_EnableBufferAttribute(int32_t location, uint16_t vert_size, FLY_VertAttrib* attr);

// Texture Attribute Management ////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLY_SetTextureParameters(const FLY_Texture2D& tex, const FLY_TexAttrib& attr);
// Overload for Texture3D or at some point try to go back to C?

// Texture Management //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLY_GenTextureData(FLY_Texture2D* tex);
FL_API void FLY_BindTexture(const FLY_Texture2D& tex);
FL_API void FLY_SetActiveTextureUnit(int32_t unit);
FL_API void FLY_SendTextureToGPU(const FLY_Texture2D& tex, int32_t mipmap_level = 0);

FL_API void FLYRENDER_ActiveTexture(int32_t texture_id);
FL_API void FLYRENDER_BindExternalTexture(int tex_type, uint32_t id);
FL_API void FLYRENDER_BindSampler(int32_t texture_locator, int32_t sampler_id);

// Shader Management ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Shader Program Management ///////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLYRENDER_CreateProgram(FLY_Program* prog);
FL_API void FLYRENDER_LinkProgram(const FLY_Program& prog);
FL_API void FLYRENDER_UseProgram(const FLY_Program& prog);

// RENDERING ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
FL_API void FLYRENDER_DrawIndices(const FLY_Mesh& mesh, int32_t offset_indices = 0, int32_t count = -1);
FL_API void FLYRENDER_DrawVertices(const FLY_Mesh& mesh, int32_t count = 0);

// Debugging
FL_API void FLYSHADER_CheckCompileLog(const FLY_Shader& fly_shader);
FL_API void FLYRENDER_CheckProgramLog(const FLY_Program& fly_program);
FL_API void CheckForRenderErrors(const char* file, int line);																			
#define FLY_CHECK_RENDER_ERRORS() FL_MACRO CheckForRenderErrors(__FILE__, __LINE__)														
			
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILESYSTEM //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//  i don't like having to inlcude std::stirng..........
#include <string>

enum FLY_Assets
{

};

typedef struct FLY_Dir
{
	FLY_Dir* parent;
	std::vector<FLY_Dir*> children;

	const char* name;

	char* native_dir_data;
} FLY_Dir;

typedef struct FLY_Asset
{
	uint16_t type;
	int64_t size;
	char* raw_data;
}	FLY_Asset;

FL_API std::string FLYFS_GetExecPath();
// On Android, all these functions require permissions
FL_API bool FLYFS_CreateDirFullPath(const char* path);
FL_API bool FLYFS_CreateDirRelative(const char* path, int32_t flags = NULL);
FL_API FLY_Asset* FLYFS_LoadFileRaw(const char* path);
FL_API bool FLYFS_WriteFileRaw(const char* path, char* data);

// Permission safe functions
FL_API FLY_Dir* FLYFS_CreateBaseDirTree();
FL_API void FLYFS_UpdateTree(FLY_Dir* root);
FL_API FLY_Dir* FLYFS_GetDirInTree(FLY_Dir* root, const char* leaf);
FL_API FLY_Asset* FLYFS_GetAssetRaw(FLY_Dir* start_dir, const char* file);
#endif // _FLY_LIB_ ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////