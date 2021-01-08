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
static FLY_RenderState  fly_renderState;
static FLY_Program      fly_shaderProgram;
static FLY_Mesh         fly_dataHandle;
static FLY_Uniform      fly_attrTex;
static FLY_Uniform      fly_attrProjMtx;
// Save User Render State

// Shaders
const char* vertex_shader =
    "precision mediump float;\n"
    "in vec2 v_pos;\n"
    "in vec2 v_uv;\n"
    "in vec4 v_col;\n"
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "uniform mat4 ProjMtx;\n"
    "void main()\n"
    "{\n"
    "	Frag_UV = v_uv;\n"
    "	Frag_Color = v_col;\n"
    "	gl_Position = ProjMtx * vec4(v_pos.xy, 0, 1);\n"
    "}\n";

const char* fragment_shader =
    "precision mediump float;\n"
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "uniform sampler2D Texture;\n"
    "void main()\n"
    "{\n"
    "	gl_FragColor = Frag_Color * texture2D( Texture, Frag_UV.st);\n"
    "}\n";

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
void ImGui_ImplFlyLib_CreateShaderProgram()
{
    fly_renderState.BackUp();
    
    fly_dataHandle.PrepareBuffers(1);
    fly_dataHandle.buffers[0]->SetComponentSize(2,4,2,0);
    

    FLY_Shader* vert = FLYSHADER_Create(FLY_VERTEX_SHADER, vertex_shader);
    vert->Compile();
    FLY_Shader* frag = FLYSHADER_Create(FLY_FRAGMENT_SHADER, fragment_shader);
    frag->Compile();

    fly_shaderProgram.Init(NULL);
    fly_shaderProgram.AttachShader(&vert);
    fly_shaderProgram.AttachShader(&frag);

    /*
    fly_shaderProgram.AttachShader(&FLYSHADER_Create(FLYSHADER_VERTEX, vertex_shader));
    fly_shaderProgram.AttachShader(&FLYSHADER_Create(FLYSHADER_VERTEX, fragment_shader));
    fly_shaderProgram.CompileShaders();
    */

    fly_shaderProgram.Link();
    fly_attrTex.name = "Texture";
    fly_attrProjMtx.name = "ProjMtx";
    fly_shaderProgram.uniform.push_back(&fly_attrTex);
    fly_shaderProgram.uniform.push_back(&fly_attrProjMtx);

    fly_shaderProgram.SetupUniformLocations();
    fly_shaderProgram.EnableAttributes(fly_dataHandle.buffers[0]);
    fly_renderState.SetUp();
}

void ImGui_ImplFlyLib_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();

    // Build Texture Atlas
    fly_fontTexture.Init(FLY_RGBA);
    io.Fonts->GetTexDataAsRGBA32(&fly_fontTexture.pixels, &fly_fontTexture.w, &fly_fontTexture.h);

    // Create the Texture
    fly_renderState.BackUp();
    fly_fontTexture.SetFiltering(FLY_LINEAR, FLY_LINEAR);
    fly_fontTexture.SendToGPU();

    // ImGui stores texture_id
    io.Fonts->TexID = (void*)(intptr_t)fly_fontTexture.id;

    fly_renderState.SetUp();
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
	bool no_button_active;
	for(int i = 0; i < IM_ARRAYSIZE(fly_MousePressed); ++i)
	{
		FLYINPUT_Actions state = FLYINPUT_GetMouseButton(i);
		fly_MousePressed_last[i] = fly_MousePressed[i];
		fly_MousePressed[i] = !(state == FLY_ACTION_RELEASE || state == FLY_ACTION_UNKNOWN);
		no_button_active = (no_button_active == true && fly_MousePressed[i] == false);
	}
	fly_MousePos = (no_button_active && fly_NoButtonActiveLast) ? ImVec2(-FLT_MAX, -FLT_MAX) : fly_MousePos;
	fly_NoButtonActiveLast = no_button_active;
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

bool ImGui_ImpFlyLib_Init()
{
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

    // SetRenderDrawLists Func
    io.RenderDrawListsFn = ImGui_ImplFlyLib_RenderDrawListsFn;
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
    io.DeltaTime = fly_lastTime - curr_time;
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