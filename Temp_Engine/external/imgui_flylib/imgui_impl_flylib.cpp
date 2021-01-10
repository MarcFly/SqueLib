#include "imgui_impl_flylib.h"

#include <fly_lib.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VAR DEFS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timing
static FLY_Timer    fly_Time;
static float        fly_lastTime = 0;
// Drawing Variables
static uint16           fly_displayWidth = 0;
static uint16           fly_displayHeight = 0;
static uint32           fly_FontTexture = 0;
static FLY_Texture2D    fly_fontTexture;
static FLY_RenderState  fly_backupState;
static FLY_RenderState  fly_renderState;
static FLY_Program      fly_shaderProgram;
static FLY_Mesh         fly_dataHandle;
// Save User Render State

// Shaders
const char* vertex_shader_source =
    "layout (location = 0) in vec2 aPos;\n"
    "layout (location = 1) in vec2 aUV;\n"
    "layout (location = 2) in vec4 aColor;\n"
    "out vec3 v_col;\n"
    "out vec2 uv;\n"
    "void main()\n"
    "{\n"
    "   uv = aUV;\n"
    "   v_col = aColor.xyz;\n"
    "   gl_Position = vec4(aPos.x, aPos.y, 0., 1.0);\n"
    "}\0";

const char* frag_shader_source =
    "out vec4 FragColor;\n"
    "in vec3 v_col;\n"
    "in vec2 uv;\n"
    "uniform vec4 ourColor;\n"
    "void main() { FragColor = ourColor+vec4(v_col, 1.0)+vec4(uv,0.,1.);}\0";

// Input Variables
static bool			fly_KeyCtrl = false;
static bool			fly_KeyShift = false;
static bool			fly_KeyAlt = false;
static bool			fly_KeySuper = false;
static ImVec2		fly_MousePos = {-FLT_MAX, -FLT_MAX};
static bool			fly_NoButtonActiveLast = false;
static bool         fly_MousePressed_last[5] = { false, false, false, false, false };
static bool         fly_MousePressed[5] = { false, false, false, false, false };
static float        fly_MouseWheel = 0.0f;
static float		fly_MouseWheelH = 0.0f;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ORGANIZATION FUNCTIONS ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING
void ImGui_ImplFlyLib_RenderDrawListsFn(ImDrawData* draw_data)
{

}

void ImGui_ImplFlyLib_PrepareBuffers()
{
    
}

void ImGui_ImplFlyLib_CreateShaderProgram()
{

}

void ImGui_ImplFlyLib_CreateFontsTexture()
{
	
}


// INPUT PROCESSING
void ImGui_ImplFlyLib_UpdateMousePosAndButtons()
{
	// Update Position
	// Get if window is focused and so something about it
	FLYINPUT_GetMousePos(&fly_MousePos.x, &fly_MousePos.y);

	// Update Wheel
	fly_MouseWheel = 0.0f;
	fly_MouseWheelH = 0.0f;

	// Update Modifiers
	fly_KeyCtrl = false;
	fly_KeyShift = false;
	fly_KeyAlt = false;
	fly_KeySuper = false;

	// Update Mouse / Pointer / MouseButtons / ScrollWheel
	for(int i = 0; i < IM_ARRAYSIZE(fly_MousePressed); ++i)
	{
		FLYINPUT_Actions state = FLYINPUT_GetMouseButton(i);
		fly_MousePressed_last[i] = fly_MousePressed[i];
		fly_MousePressed[i] = !(state == FLY_ACTION_RELEASE || state == FLY_ACTION_UNKNOWN);
	}
}

const char* ImGui_ImplFlyLib_GetClipboardText()
{
    return "TODO...";
}

void ImGui_ImplFlyLib_SetClipboardText(const char* text)
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC USAGE FUNCTIONS ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Has Errors
bool ImGui_ImplFlyLib_Init()
{
    //gladLoadGL();
    fly_Time.Start();
    bool ret = true;

    ImGuiIO &io = ImGui::GetIO();
    // Init KeyMap
    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_Tab] = FLY_KEY_TAB;                     
	io.KeyMap[ImGuiKey_LeftArrow] = FLY_KEY_ARROW_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = FLY_KEY_ARROW_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = FLY_KEY_ARROW_UP;
	io.KeyMap[ImGuiKey_DownArrow] = FLY_KEY_ARROW_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = FLY_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = FLY_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = FLY_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = FLY_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = FLY_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = FLY_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = FLY_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = FLY_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = FLY_KEY_UPPER_A;
	io.KeyMap[ImGuiKey_C] = FLY_KEY_UPPER_C;
	io.KeyMap[ImGuiKey_V] = FLY_KEY_UPPER_V;
	io.KeyMap[ImGuiKey_X] = FLY_KEY_UPPER_X;
	io.KeyMap[ImGuiKey_Y] = FLY_KEY_UPPER_Y;
	io.KeyMap[ImGuiKey_Z] = FLY_KEY_UPPER_Z;

    // Init Scale Based on DPI (have to tune it)
    uint16 dpi = FLYDISPLAY_GetDPIDensity();
	uint16 w, h; FLYDISPLAY_GetSize(0, &w, &h);
	uint16 bigger = (w > h) ? w : h;
	float scale = (bigger / dpi)*.8;
	io.FontGlobalScale = scale;
	ImGui::GetStyle().ScaleAllSizes(scale);

    // BackendFlags and things...
    //io.BackendFlags &= ~ImGuiBackendFlags_HasMouseCursors;
	//io.BackendFlags &= ~ImGuiBackendFlags_HasSetMousePos;
	io.BackendPlatformName = "imgui_impl_flylib";

    // Setup Renderer
    fly_renderState.blend = true;
    fly_renderState.blend_equation_rgb = fly_renderState.blend_equation_alpha = FLY_FUNC_ADD;
    fly_renderState.blend_func_src_alpha = FLY_SRC_ALPHA;
    fly_renderState.blend_func_dst_alpha = FLY_ONE_MINUS_SRC_ALPHA;
    fly_renderState.cull_faces = false;
    fly_renderState.depth_test = false;
    fly_renderState.scissor_test = true;
    fly_renderState.polygon_mode = int2(FLY_FRONT_AND_BACK, FLY_FILL);

    io.RenderDrawListsFn = ImGui_ImplFlyLib_RenderDrawListsFn;
    ImGui_ImplFlyLib_PrepareBuffers();
    ImGui_ImplFlyLib_CreateShaderProgram();
    ImGui_ImplFlyLib_CreateFontsTexture();
    return ret;
}

void ImGui_ImplFlyLib_Shutdown()
{

}

void ImGui_ImplFlyLib_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
	FLYDISPLAY_GetSize(0, &fly_displayWidth, &fly_displayHeight);
    io.DisplaySize = ImVec2(fly_displayWidth, fly_displayHeight);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    // Setup Time Step
    float curr_time = (fly_Time.ReadMilliSec() / 1000.f);
    io.DeltaTime = curr_time - fly_lastTime;
    fly_lastTime = curr_time;

    // Update Mouse
    ImGui_ImplFlyLib_UpdateMousePosAndButtons();

    for(int i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
		io.MouseDown[i] = fly_MousePressed[i];

	io.MousePos = fly_MousePos;
	
    io.MouseWheel   = fly_MouseWheel;
	io.MouseWheelH  = fly_MouseWheelH;
	
    io.KeyCtrl  = fly_KeyCtrl; 
	io.KeyShift = fly_KeyShift; 
	io.KeyAlt   = fly_KeyAlt; 
	io.KeySuper = fly_KeySuper;

	int code;
	while((code = FLYINPUT_GetCharFromBuffer()) != -1)
		io.AddInputCharacter(code);

    // More Input Updates? ...
}