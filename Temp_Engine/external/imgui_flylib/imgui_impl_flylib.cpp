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
const char* vertex_shader =
    "in vec2 v_pos;\n"
    "in vec4 v_color;\n"
    "in vec2 v_uv;\n"    
    "out vec2 Frag_UV;\n"
    "out vec4 Frag_Color;\n"
    "uniform mat4 ProjMtx;\n"
    "void main()\n"
    "{\n"
    "	Frag_UV = v_uv;\n"
    "	Frag_Color = v_color;\n"
    "	gl_Position = ProjMtx * vec4(v_pos.xy, 0, 1);\n"
    "}\n";

const char* fragment_shader =
    "in vec2 Frag_UV;\n"
    "in vec4 Frag_Color;\n"
    "uniform sampler2D Texture;\n"
    "void main()\n"
    "{\n"
    "	gl_FragColor = Frag_Color * texture2D( Texture, Frag_UV.st);\n"
    "}\n";

const char* vertex_shader_core =
"layout (location = 0) in vec2 v_pos;\n"
"layout (location = 1) in vec4 v_color;\n"
"layout (location = 2) in vec2 v_uv;\n"
"out vec2 Frag_UV;\n"
"out vec4 Frag_Color;\n"
"uniform mat4 ProjMtx;\n"
"void main()\n"
"{\n"
"	Frag_UV = v_uv;\n"
"	Frag_Color = v_color;\n"
"	gl_Position = ProjMtx * vec4(v_pos.xy, 0, 1);\n"
"}\n";

const char* fragment_shader_core =
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
void ImGui_ImplFlyLib_RenderDrawListsFn(ImDrawData* draw_data)
{
    ImGuiIO& io = ImGui::GetIO();
    // BackUp State
    if (glGetError() > 0)
    {
        bool break_here = 0;
    }
    fly_backupState.BackUp();
    // Apply new state
    fly_renderState.SetUp();
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Setup Ortho Projection Matrix
    FLYRENDER_ChangeViewPortSize(io.DisplaySize.x, io.DisplaySize.y);
    const float ortho_projection[4][4] =
    {
        { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        {-1.0f,                  1.0f,                   0.0f, 1.0f },
    };

    fly_shaderProgram.Use();
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    fly_shaderProgram.SetInt("Texture", 0);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    fly_shaderProgram.SetMatrix4("ProjMtx", &ortho_projection[0][0]);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    for (int i = 0; i < draw_data->CmdListsCount; ++i)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[i];
        const ImDrawIdx* idx_buffer_offset = 0;

        fly_dataHandle.verts = (char*)cmd_list->VtxBuffer.Data;
        fly_dataHandle.num_verts = cmd_list->VtxBuffer.size();
        fly_dataHandle.indices = (char*)cmd_list->IdxBuffer.Data;
        fly_dataHandle.num_index = cmd_list->IdxBuffer.size();

        FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
        fly_dataHandle.SendToGPU();
        fly_shaderProgram.EnableOwnAttributes();
        FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
        for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); ++pcmd)
        {
            if (pcmd->UserCallback)
                pcmd->UserCallback(cmd_list, pcmd);
            else
            {
                FLYRENDER_BindExternalTexture(FLY_TEXTURE_2D, (uint32)(intptr_t)pcmd->TextureId);
                FLYRENDER_Scissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                fly_shaderProgram.DrawIndices(&fly_dataHandle, pcmd->IdxOffset, pcmd->ElemCount);
            }
            idx_buffer_offset += pcmd->ElemCount;
            FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
        }
    }

    // Regain State
    fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateShaderProgram()
{
    // Copy State
    fly_backupState.BackUp();
    

    fly_dataHandle.SetDrawMode(FLY_STREAM_DRAW);
    fly_dataHandle.SetIndexVarType((sizeof(ImDrawIdx) == 2) ? FLY_USHORT : FLY_UINT);
    fly_dataHandle.Prepare();
    // Initialize the Attributes
    FLY_Attribute* pos = new FLY_Attribute();
    pos->SetName("v_pos");
    pos->SetVarType(FLY_FLOAT);
    pos->SetNumComponents(2);
    pos->SetNormalize(false);
    FLY_Attribute* pos_ = new FLY_Attribute();
    *pos_ = *pos;

    FLY_Attribute* uv = new FLY_Attribute();
    uv->SetName("v_uv");
    uv->SetVarType(FLY_FLOAT);
    uv->SetNumComponents(2);
    uv->SetNormalize(false);
    uv->SetOffset(pos->GetSize());
    FLY_Attribute* uv_ = new FLY_Attribute();
    *uv_ = *uv;

    FLY_Attribute* color = new FLY_Attribute();
    color->SetName("v_color");
    color->SetVarType(FLY_UBYTE);
    color->SetNumComponents(4);
    color->SetNormalize(true);
    color->SetOffset(uv->offset + uv->GetSize());
    FLY_Attribute* color_ = new FLY_Attribute();
    *color_ = *color;
    
    fly_dataHandle.GiveAttribute(&pos_);
    fly_dataHandle.GiveAttribute(&uv_);
    fly_dataHandle.GiveAttribute(&color_);
    // Initialize the Shaders
    const char* vert_shader[2] = {FLYRENDER_GetGLSLVer(),vertex_shader_core};
    const char* frag_shader[2] = {FLYRENDER_GetGLSLVer(), fragment_shader_core };
    FLY_Shader* vert = FLYSHADER_Create(FLY_VERTEX_SHADER, 2, vert_shader, true);
    vert->Compile();
    FLY_Shader* frag = FLYSHADER_Create(FLY_FRAGMENT_SHADER, 2, frag_shader, true);
    frag->Compile();

    fly_shaderProgram.Init();
    fly_shaderProgram.AttachShader(&vert);
    fly_shaderProgram.AttachShader(&frag);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    /*
    fly_shaderProgram.AttachShader(&FLYSHADER_Create(FLYSHADER_VERTEX, vertex_shader));
    fly_shaderProgram.AttachShader(&FLYSHADER_Create(FLYSHADER_VERTEX, fragment_shader));
    fly_shaderProgram.CompileShaders();
    */

    fly_shaderProgram.Link();
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());

    fly_shaderProgram.DeclareUniform("Texture");
    fly_shaderProgram.DeclareUniform("ProjMtx");
    fly_shaderProgram.SetupUniformLocations();

    fly_shaderProgram.Use();
    fly_shaderProgram.GiveAttribute(&pos);
    fly_shaderProgram.GiveAttribute(&uv);
    fly_shaderProgram.GiveAttribute(&color);

    //fly_shaderProgram.EnableAttributes(fly_dataHandle.buffers[0]);

    // Go back to the latest State
    fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();

    // Build Texture Atlas
    fly_fontTexture.Init(FLY_RGBA);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    io.Fonts->GetTexDataAsRGBA32(&fly_fontTexture.pixels, &fly_fontTexture.w, &fly_fontTexture.h);

    // Create the Texture
    fly_backupState.BackUp();
    fly_fontTexture.SetFiltering(FLY_LINEAR, FLY_LINEAR);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    fly_fontTexture.SendToGPU();
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());

    // ImGui stores texture_id
    io.Fonts->TexID = (void*)(intptr_t)fly_fontTexture.id;

    fly_backupState.SetUp();
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

    io.RenderDrawListsFn = ImGui_ImplFlyLib_RenderDrawListsFn;
    ImGui_ImplFlyLib_CreateShaderProgram();
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    ImGui_ImplFlyLib_CreateFontsTexture();
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
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