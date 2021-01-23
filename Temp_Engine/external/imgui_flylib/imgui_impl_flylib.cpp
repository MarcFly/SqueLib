#include "imgui_impl_flylib.h"

#include <fly_lib.h>
#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VAR DEFS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timing
static FLY_Timer    fly_Time;
static double        fly_lastTime = 0;
// Input Variables
static bool			fly_wantKeyboard = false;
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

static KeyCallback			fly_PrevKeyboardCallback;
static MouseFloatCallback	fly_PrevMousePosCallback;
static MouseFloatCallback	fly_PrevMouseScrollCallback;
static VoidFun				fly_PrevOnResumeCallback;
static VoidFun				fly_PrevOnGoBackgroundCallback;
// Shaders
const char* vertex_shader =
	"layout (location = 0) in vec2 Position;\n"
	"layout (location = 1) in vec2 UV;\n"
	"layout (location = 2) in vec4 Color;\n"
	"uniform mat4 ProjMtx;\n"
	"out vec2 Frag_UV;\n"
	"out vec4 Frag_Color;\n"
	"void main()\n"
	"{\n"
	"    Frag_UV = UV;\n"
	"    Frag_Color = Color;\n"
	"    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
	"}\n";
const char* fragment_shader =
	"in vec2 Frag_UV;\n"
	"in vec4 Frag_Color;\n"
	"uniform sampler2D Texture;\n"
	"layout (location = 0) out vec4 Out_Color;\n"
	"void main()\n"
	"{\n"
	"    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
	"}\n";

// Drawing Variables
static uint16_t         fly_displayWidth = 0;
static uint16_t         fly_displayHeight = 0;
static FLY_Texture2D    fly_fontTexture;
static FLY_RenderState  fly_backupState;
static FLY_RenderState  fly_renderState;
static FLY_Program      fly_shaderProgram;
static FLY_Mesh         fly_dataHandle;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImGui_ImplFlyLib_VariableRenderState(ImDrawData* draw_data, int32_t  fb_width, int32_t fb_height)
{
	fly_renderState.viewport[2] = fb_width;
	fly_renderState.viewport[3] = fb_height;
	fly_renderState.SetUp();

	float L = draw_data->DisplayPos.x;
	float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
	float T = draw_data->DisplayPos.y;
	float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	
	const float ortho_projection[4][4] =
	{
		{ 2.0f / (R - L),		0.0f,				0.0f,   0.0f },
		{ 0.0f,					2.0f / (T - B),		0.0f,   0.0f },
		{ 0.0f,					0.0f,			   -1.0f,   0.0f },
		{ (R + L) / (L - R),	(T + B) / (B - T),  0.0f,   1.0f }
	};

	FLYRENDER_UseProgram(fly_shaderProgram);
	SetInt(fly_shaderProgram, "Texture", 0);
	SetMatrix4(fly_shaderProgram, "ProjMtx", &ortho_projection[0][0]);
	
	/*FLY_BindMeshBuffer(fly_dataHandle);
	fly_dataHandle.EnableAttributesForProgram(fly_shaderProgram);*/
}

void ImGui_ImplFlyLib_Render(ImDrawData* draw_data)
{
	fly_backupState.BackUp();

	int32_t fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int32_t fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0) return;

	ImGui_ImplFlyLib_VariableRenderState(draw_data, fb_width, fb_height);

	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	for (int i = 0; i < draw_data->CmdListsCount; ++i)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[i];
		fly_dataHandle.ChangeVertData(cmd_list->VtxBuffer.Size, cmd_list->VtxBuffer.Data);
		fly_dataHandle.ChangeIndexData(cmd_list->IdxBuffer.Size, cmd_list->IdxBuffer.Data);
		
		FLY_SendMeshToGPU(fly_dataHandle);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if(pcmd->UserCallback != NULL)
			{ }
			else
			{
				// Project scissor/clipping rectangles into framebuffer space
				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;
				if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
				{
					FLYRENDER_Scissor((int32_t)clip_rect.x, (int32_t)(fb_height - clip_rect.w), (int32_t)(clip_rect.z - clip_rect.x), (int32_t)(clip_rect.w - clip_rect.y));
					FLYRENDER_BindExternalTexture(FLY_TEXTURE_2D, (uint32_t)(intptr_t)pcmd->TextureId);
					FLYRENDER_DrawIndices(fly_dataHandle, pcmd->IdxOffset, pcmd->ElemCount);
				}
			}
		}

	}
	fly_dataHandle.ChangeVertData(0, NULL);
	fly_dataHandle.ChangeIndexData(0, NULL);

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_PrepareBuffers()
{
	fly_backupState.BackUp();

	FLY_GenerateMeshBuffer(&fly_dataHandle);
	fly_dataHandle.draw_config = FLY_TRIANGLES;
	fly_dataHandle.draw_mode = FLY_STREAM_DRAW;
	fly_dataHandle.index_var = FLY_USHORT;	
	fly_dataHandle.index_var_size = FLY_VarGetSize(FLY_USHORT);

	FLY_VertAttrib* p = fly_dataHandle.AddAttribute();
	p->name = "Position";
	p->normalize = false;
	p->num_comp = 2;
	p->var_type = FLY_FLOAT;
	p->var_size = FLY_VarGetSize(FLY_FLOAT);

	FLY_VertAttrib* uv = fly_dataHandle.AddAttribute();
	uv->name = "UV";
	uv->normalize = false;
	uv->num_comp = 2;
	uv->var_type = FLY_FLOAT;
	uv->var_size = FLY_VarGetSize(FLY_FLOAT);

	FLY_VertAttrib* c = fly_dataHandle.AddAttribute();
	c->name = "Color";
	c->normalize = true;
	c->num_comp = 4;
	c->var_type = FLY_UBYTE;
	c->var_size = FLY_VarGetSize(FLY_UBYTE);

	fly_dataHandle.SetLocationsInOrder();

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateShaderProgram()
{
	fly_backupState.BackUp();

	const char* vert_source[2] = { FLYRENDER_GetGLSLVer(), vertex_shader };
	FLY_Shader* vert_s = new FLY_Shader(FLY_VERTEX_SHADER, 2, vert_source);
	vert_s->Compile();

	const char* frag_source[2] = { FLYRENDER_GetGLSLVer(), fragment_shader };
	FLY_Shader* frag_s = new FLY_Shader(FLY_FRAGMENT_SHADER, 2, frag_source);
	frag_s->Compile();

	FLYRENDER_CreateProgram(&fly_shaderProgram);
	fly_shaderProgram.AttachShader(vert_s);
	fly_shaderProgram.AttachShader(frag_s);

	FLYRENDER_LinkProgram(fly_shaderProgram);
	fly_shaderProgram.DeclareUniform("Texture");
	fly_shaderProgram.DeclareUniform("ProjMtx");

	ImGui_ImplFlyLib_PrepareBuffers();

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateFontsTexture()
{
	fly_backupState.BackUp();

	ImGuiIO& io = ImGui::GetIO();
	FLY_GenTextureData(&fly_fontTexture);
	fly_fontTexture.var_type = FLY_UBYTE;
	fly_fontTexture.var_size = 1;
	fly_fontTexture.format = FLY_RGBA;
	io.Fonts->GetTexDataAsRGBA32((uchar**)&fly_fontTexture.pixels, &fly_fontTexture.w, &fly_fontTexture.h);

	FLY_TexAttrib* min_filter = new FLY_TexAttrib(FLY_MIN_FILTER, FLY_INT, new int32_t(FLY_LINEAR));
	fly_fontTexture.SetParameter(min_filter);

	FLY_TexAttrib* mag_filter = new FLY_TexAttrib(FLY_MAG_FILTER, FLY_INT, new int32_t(FLY_LINEAR));
	fly_fontTexture.SetParameter(mag_filter);

	fly_fontTexture.ApplyParameters();
	FLY_SendTextureToGPU(fly_fontTexture);

	io.Fonts->TexID = (ImTextureID)(intptr_t)fly_fontTexture.id;

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_StaticRenderState()
{
	fly_backupState.BackUp();

	fly_renderState.BackUp();

	fly_renderState.blend = true;
	fly_renderState.blend_equation_rgb = FLY_FUNC_ADD;
	fly_renderState.blend_equation_alpha = FLY_FUNC_ADD;
	fly_renderState.cull_faces = false;
	fly_renderState.depth_test = false;
	fly_renderState.scissor_test = true;
	fly_renderState.active_texture_unit = FLY_TEXTURE0;
	fly_renderState.blend_func_dst_alpha = FLY_ONE;
	fly_renderState.blend_func_src_alpha = FLY_ONE;
	fly_renderState.blend_func_dst_rgb = FLY_ONE_MINUS_SRC_ALPHA;
	fly_renderState.blend_func_src_rgb = FLY_SRC_ALPHA;
	fly_renderState.blend_func_separate = true;
	fly_renderState.polygon_mode[0] = FLY_FILL;
	fly_renderState.polygon_mode[1] = FLY_FILL;

	fly_backupState.SetUp();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImGui_ImplFlyLib_MousePosCallback(float x, float y)
{
	fly_MousePos.x = x;
	fly_MousePos.y = y;
	fly_PrevMousePosCallback(x, y);
}
void ImGui_ImplFlyLib_MouseScrollCallback(float x, float y)
{
	fly_MouseWheel = y;
	fly_MouseWheelH = x;
	fly_PrevMouseScrollCallback(x, y);
}

void ImGui_ImplFlyLib_UpdateMouseButtons()
{

	// Update Mouse / Pointer / MouseButtons / ScrollWheel
	for(int i = 0; i < IM_ARRAYSIZE(fly_MousePressed); ++i)
	{
		FLYINPUT_Actions state = FLYINPUT_GetMouseButton(i);
		fly_MousePressed_last[i] = fly_MousePressed[i];
		fly_MousePressed[i] = !(state == FLY_ACTION_RELEASE || state == FLY_ACTION_UNKNOWN);
	}
}

void ImGui_ImplFlyLib_KeyboardCallback(int key, int state)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = (state > FLY_RELEASED) ? true : false;
	fly_PrevKeyboardCallback(key, state);
}

const char* ImGui_ImplFlyLib_GetClipboardText()
{
    return "TODO...";
}

void ImGui_ImplFlyLib_SetClipboardText(const char* text)
{

}

void ImGui_ImplFlyLib_Resume()
{
	ImGui_ImplFlyLib_Init();
	if (fly_PrevOnResumeCallback != NULL) fly_PrevOnResumeCallback();
}

void ImGui_ImplFlyLib_GoBackground()
{

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;
	fly_shaderProgram.CleanUp();

	fly_dataHandle.CleanUp();
	fly_fontTexture.CleanUp();

	if (fly_PrevOnGoBackgroundCallback != NULL) fly_PrevOnGoBackgroundCallback();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC USAGE FUNCTIONS ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Has Errors

bool init_registered = false;
void ImGui_ImplFlyLib_Init()
{
	fly_Time.Start();

	if(!init_registered)
	{
		fly_PrevMousePosCallback = FLYINPUT_SetMousePosCallback(ImGui_ImplFlyLib_MousePosCallback);
		fly_PrevMouseScrollCallback = FLYINPUT_SetMouseScrollCallback(ImGui_ImplFlyLib_MouseScrollCallback);
		fly_PrevKeyboardCallback = FLYINPUT_SetKeyCallback(ImGui_ImplFlyLib_KeyboardCallback);
		fly_PrevOnResumeCallback = FLYINPUT_AddOnResumeCallback(ImGui_ImplFlyLib_Resume);
		fly_PrevOnGoBackgroundCallback = FLYINPUT_AddOnGoBackgroundCallback(ImGui_ImplFlyLib_GoBackground);
		init_registered = true;
	}
	
    ImGuiIO &io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
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
    uint16_t dpi = FLYDISPLAY_GetDPIDensity();
	uint16_t w, h; FLYDISPLAY_GetMainDisplaySize(w, h);
	uint16_t bigger = (w > h) ? w : h;
	float scale = (bigger / dpi)/2.3f;
	io.FontGlobalScale = scale;
	ImGui::GetStyle().ScaleAllSizes(scale);

    // BackendFlags and things...
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	//io.BackendFlags &= ~ImGuiBackendFlags_HasSetMousePos;
	io.BackendPlatformName = "imgui_impl_flylib";
	
	// how will it work with multiple windows and multiple viewports?
	// Do the multiwindows have to share the context?
	// Does ImGui have to be initialized on a per window basis?
	io.ImeWindowHandle = FLYDISPLAY_GetPlatformWindowHandle(0);
	
	
	// Initialize Render State...
	ImGui_ImplFlyLib_StaticRenderState();
	ImGui_ImplFlyLib_CreateFontsTexture();
	ImGui_ImplFlyLib_CreateShaderProgram();

	if(init_registered && fly_PrevOnResumeCallback != NULL) fly_PrevOnResumeCallback();
}

void ImGui_ImplFlyLib_Shutdown()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;
	fly_shaderProgram.CleanUp();

	fly_dataHandle.CleanUp();
	fly_fontTexture.CleanUp();

	FLYINPUT_SetMousePosCallback(fly_PrevMousePosCallback);
	FLYINPUT_SetMouseScrollCallback(fly_PrevMouseScrollCallback);
	FLYINPUT_SetKeyCallback(fly_PrevKeyboardCallback);
	FLYINPUT_AddOnResumeCallback(fly_PrevOnResumeCallback);
	FLYINPUT_AddOnGoBackgroundCallback(fly_PrevOnGoBackgroundCallback);

	init_registered = false;
}

void ImGui_ImplFlyLib_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

	if(fly_wantKeyboard != io.WantTextInput) 
	{
		FLYINPUT_DisplaySoftwareKeyboard(io.WantTextInput);
		fly_wantKeyboard = io.WantTextInput;
	}
	IM_ASSERT(io.Fonts->IsBuilt());

	int32_t w, h;
	FLYDISPLAY_GetWindowSize(0, &w, &h); 
	w = (w <= 0) ? 1 : w;
	h = (h <= 0) ? 1 : h;
	io.DisplaySize = ImVec2(w, h);
	FLYDISPLAY_GetViewportSize(0, &w, &h);
	FLYPRINT(LT_INFO, "%d,%d %d,%d", w,h, io.DisplaySize.x, io.DisplaySize.y);
	io.DisplayFramebufferScale = ImVec2(((float)w/ io.DisplaySize.x), ( (float)h / io.DisplaySize.y));

    // Setup Time Step
	double read = fly_Time.ReadMilliSec();
    double curr_time = (read / 1000);
    io.DeltaTime = (curr_time - fly_lastTime)+0.000001;
    fly_lastTime = curr_time;

    // Update Mouse
	ImGui_ImplFlyLib_UpdateMouseButtons();

    for(int i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
		io.MouseDown[i] = fly_MousePressed[i];

	
	io.MousePos = fly_MousePos;
	
    io.MouseWheel   = fly_MouseWheel;
	io.MouseWheelH  = fly_MouseWheelH;
	fly_MouseWheel = fly_MouseWheelH = 0;

    io.KeyCtrl  = io.KeysDown[FLY_KEY_LEFT_CTRL] || io.KeysDown[FLY_KEY_RIGHT_CTRL]; 
	io.KeyShift = io.KeysDown[FLY_KEY_LEFT_SHIFT] || io.KeysDown[FLY_KEY_RIGHT_SHIFT];
	io.KeyAlt   = io.KeysDown[FLY_KEY_LEFT_ALT] || io.KeysDown[FLY_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[FLY_KEY_LEFT_SUPER] || io.KeysDown[FLY_KEY_RIGHT_SUPER];;

	int code;
	while((code = FLYINPUT_GetCharFromBuffer()) != -1)
		io.AddInputCharacter(code);

    // More Input Updates? ...
}