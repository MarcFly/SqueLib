#include "imgui_impl_flylib.h"

#include <fly_lib.h>


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VAR DEFS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timing
static FLY_Timer    fly_Time;
static float        fly_lastTime = 0;
// Input Variables
static bool			fly_KeyCtrl = false;
static bool			fly_KeyShift = false;
static bool			fly_KeyAlt = false;
static bool			fly_KeySuper = false;
static ImVec2		fly_MousePos = { -FLT_MAX, -FLT_MAX };
static bool			fly_NoButtonActiveLast = false;
static bool         fly_MousePressed_last[5] = { false, false, false, false, false };
static bool         fly_MousePressed[5] = { false, false, false, false, false };
static float        fly_MouseWheel = 0.0f;
static float		fly_MouseWheelH = 0.0f;
// Shaders
const char* vertex_shader_source =
    "layout (location = 0) in vec2 Position;\n"
    "layout (location = 1) in vec2 UV;\n"
    "layout (location = 2) in vec4 Color;\n"
	"uniform mat4 ProjMtx;\n"
    "out vec3 v_col;\n"
    "out vec2 uv;\n"
    "void main()\n"
    "{\n"
    "   uv = UV;\n"
    "   v_col = Color.xyz;\n"
    "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
    "}\0";

const char* frag_shader_source =
    "out vec4 FragColor;\n"
	"uniform sampler2D Texture;\n"
    "in vec3 v_col;\n"
    "in vec2 uv;\n"
    "void main()\n"
	"{\n"
	"	FragColor = vec4(v_col, 1.0)*texture(Texture, uv.st);\n"
	"}\0";

// Drawing Variables
static uint16           fly_displayWidth = 0;
static uint16           fly_displayHeight = 0;
static uint32           fly_FontTexture = 0;
static FLY_Texture2D    fly_fontTexture;
static FLY_RenderState  fly_backupState;
static FLY_RenderState  fly_renderState;
static FLY_Program      fly_shaderProgram;
static FLY_Mesh         fly_dataHandle;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// ORGANIZATION FUNCTIONS ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING
void ImGui_ImplFlyLib_RenderDrawListsFn(ImDrawData* draw_data)
{
	// Check last errors
	FLY_CHECK_RENDER_ERRORS();


	fly_backupState.BackUp();

	fly_renderState.SetUp();

	ImGuiIO& io = ImGui::GetIO();

	float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	FLYRENDER_ChangeViewPortSize(io.DisplaySize.x, io.DisplaySize.y);

	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
		{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
		{-1.0f,                  1.0f,                   0.0f, 1.0f },
	};

	fly_shaderProgram.Use();
	fly_shaderProgram.SetInt("Texture", 0);
	fly_shaderProgram.SetMatrix4("ProjMtx", &ortho_projection[0][0]);

	for (int i = 0; i < draw_data->CmdListsCount; ++i)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[i];
		uint32 idx_buffer_offset = 0;

		fly_dataHandle.verts = (char*)&cmd_list->VtxBuffer.front();
		fly_dataHandle.num_verts = cmd_list->VtxBuffer.size();
		fly_dataHandle.indices = (char*)&cmd_list->IdxBuffer.front();
		fly_dataHandle.num_index = cmd_list->IdxBuffer.size();

		fly_dataHandle.SendToGPU();
		fly_dataHandle.SetAttributes();

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); ++pcmd)
		{
			if (pcmd->UserCallback)
				pcmd->UserCallback(cmd_list, pcmd);
			else
			{
				FLYRENDER_BindExternalTexture(FLY_TEXTURE_2D, (uint32)(intptr_t)pcmd->TextureId);
				FLYRENDER_Scissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				fly_shaderProgram.DrawIndices(&fly_dataHandle, idx_buffer_offset, pcmd->ElemCount);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	fly_backupState.SetUp();

	// Check last errors
	FLY_CHECK_RENDER_ERRORS();
}

void ImGui_ImplFlyLib_PrepareBuffers()
{
	fly_backupState.BackUp();

	fly_dataHandle.SetIndexVarType(FLY_USHORT);
	fly_dataHandle.Prepare();
	FLY_Attribute* pos = new FLY_Attribute();
	FLY_Attribute* uv = new FLY_Attribute();
	FLY_Attribute* color = new FLY_Attribute();
	
	pos->SetName("Position");
	uv->SetName("UV");
	color->SetName("Color");

	pos->SetVarType(FLY_FLOAT);
	uv->SetVarType(FLY_FLOAT);
	color->SetVarType(FLY_UBYTE);

	pos->SetNumComponents(2);
	uv->SetNumComponents(2);
	color->SetNumComponents(4);

	pos->SetNormalize(false);
	uv->SetNormalize(false);
	color->SetNormalize(true);

	pos->SetOffset(0);
	uv->SetOffset(pos->GetSize());
	color->SetOffset(pos->GetSize() + uv->GetSize());
	fly_dataHandle.GiveAttribute(&pos);
	fly_dataHandle.GiveAttribute(&uv);
	fly_dataHandle.GiveAttribute(&color);
	fly_dataHandle.EnableAttributesForProgram(fly_shaderProgram.id);
	fly_dataHandle.SetAttributes();

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateShaderProgram()
{
	fly_backupState.BackUp();

	const char* vert_raw[2] = { FLYRENDER_GetGLSLVer(), vertex_shader_source };
	const char* frag_raw[2] = { FLYRENDER_GetGLSLVer(), frag_shader_source };
	FLY_Shader* vert_s = FLYSHADER_Create(FLY_VERTEX_SHADER, 2, vert_raw, true);
	FLY_Shader* frag_s = FLYSHADER_Create(FLY_FRAGMENT_SHADER, 2, frag_raw, true);
	vert_s->Compile();
	frag_s->Compile();

	fly_shaderProgram.Init();
	fly_shaderProgram.AttachShader(&vert_s);
	fly_shaderProgram.AttachShader(&frag_s);
	fly_shaderProgram.Link();

	fly_shaderProgram.DeclareUniform("Texture");
	fly_shaderProgram.DeclareUniform("ProjMtx");

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateFontsTexture()
{
	fly_backupState.BackUp();

	ImGuiIO& io = ImGui::GetIO();
	
	fly_fontTexture.Init(FLY_RGBA);
	io.Fonts->GetTexDataAsRGBA32(&fly_fontTexture.pixels, &fly_fontTexture.w, &fly_fontTexture.h);
	fly_fontTexture.SetFiltering(FLY_LINEAR, FLY_LINEAR);
	fly_fontTexture.SendToGPU();
	FLY_CHECK_RENDER_ERRORS();

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
    fly_renderState.scissor_test = false;
    fly_renderState.polygon_mode = int2(FLY_FRONT_AND_BACK, FLY_FILL);

    io.RenderDrawListsFn = ImGui_ImplFlyLib_RenderDrawListsFn;
	ImGui_ImplFlyLib_CreateShaderProgram();
    ImGui_ImplFlyLib_PrepareBuffers();    
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