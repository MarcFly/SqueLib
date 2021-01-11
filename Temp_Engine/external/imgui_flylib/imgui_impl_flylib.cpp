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
const GLchar* vertex_shader =
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
const GLchar* fragment_shader =
	"uniform sampler2D Texture;\n"
	"in vec2 Frag_UV;\n"
	"in vec4 Frag_Color;\n"
	"layout (location = 0) out vec4 Out_Color;\n"
	"void main()\n"
	"{\n"
	"    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
	"}\n";

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
// RENDER FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImGui_ImplFlyLib_VariableRenderState(ImDrawData* draw_data, int32  fb_width, int32 fb_height)
{
	fly_renderState.viewport.z = fb_width;
	fly_renderState.viewport.w = fb_height;

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

	fly_shaderProgram.Use();
	fly_shaderProgram.SetInt("Texture", 0);
	fly_shaderProgram.SetMatrix4("ProjMtx", &ortho_projection[0][0]);
	FLYRENDER_BindSampler(0, 0);
}

void ImGui_ImplFlyLib_RenderDrawListsFn(ImDrawData* draw_data)
{
	int32 fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int32 fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0)	return;

	fly_backupState.BackUp();

	fly_renderState.SetUp();

	ImGui_ImplFlyLib_VariableRenderState(draw_data, fb_width, fb_height);

	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	for (int i = 0; i < draw_data->CmdListsCount; ++i)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[i];
		fly_dataHandle.verts = (void*)cmd_list->VtxBuffer.Data;
		fly_dataHandle.indices = (void*)cmd_list->IdxBuffer.Data;
		fly_dataHandle.num_verts = cmd_list->VtxBuffer.Size;
		fly_dataHandle.num_index = cmd_list->IdxBuffer.Size;
		FLY_CHECK_RENDER_ERRORS();
		fly_dataHandle.SendToGPU();
		FLY_CHECK_RENDER_ERRORS();
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
					FLYRENDER_Scissor((int32)clip_rect.x, (int32)(fb_height - clip_rect.w), (int32)(clip_rect.z - clip_rect.x), (int32)(clip_rect.w - clip_rect.y));
					FLYRENDER_BindExternalTexture(FLY_TEXTURE_2D, (uint32)pcmd->TextureId);
					FLY_CHECK_RENDER_ERRORS();
					fly_shaderProgram.DrawIndices(&fly_dataHandle, pcmd->IdxOffset, pcmd->ElemCount);
				}
			}
		}

	}
	fly_dataHandle.verts = NULL;
	fly_dataHandle.indices = NULL;
	fly_dataHandle.num_verts = 0;
	fly_dataHandle.num_index = 0;



	fly_backupState.SetUp();

	FLY_CHECK_RENDER_ERRORS();
}

void ImGui_ImplFlyLib_PrepareBuffers()
{
	fly_backupState.BackUp();

	fly_dataHandle.SetDrawMode(FLY_STREAM_DRAW);
	fly_dataHandle.SetIndexVarType(FLY_USHORT);
	fly_dataHandle.Init();

	FLY_VertAttrib* p = fly_dataHandle.AddAttribute();
	p->SetName("Position");
	p->SetNormalize(false);
	p->SetNumComponents(2);
	p->SetVarType(FLY_FLOAT);
	
	FLY_VertAttrib* uv = fly_dataHandle.AddAttribute();
	uv->SetName("UV");
	uv->SetNormalize(true);
	uv->SetNumComponents(2);
	uv->SetVarType(FLY_FLOAT);

	FLY_VertAttrib* c = fly_dataHandle.AddAttribute();
	c->SetName("Color");
	c->SetNormalize(true);
	c->SetNumComponents(4);
	c->SetVarType(FLY_UBYTE);

	fly_dataHandle.SetOffsetsInOrder();
	fly_dataHandle.SetLocationsInOrder();

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateShaderProgram()
{
	fly_backupState.BackUp();

	const char* vert_source[2] = { FLYRENDER_GetGLSLVer(), vertex_shader };
	FLY_Shader* vert_s = FLYSHADER_Create(FLY_VERTEX_SHADER, 2, vert_source, true);
	vert_s->Compile();

	const char* frag_source[2] = { FLYRENDER_GetGLSLVer(), fragment_shader };
	FLY_Shader* frag_s = FLYSHADER_Create(FLY_FRAGMENT_SHADER, 2, frag_source, true);
	frag_s->Compile();

	// Remember to revise attach shader to AddShader (like attributes)
	fly_shaderProgram.Init();
	fly_shaderProgram.AttachShader(&vert_s);
	fly_shaderProgram.AttachShader(&frag_s);
	// Compile Shaders with AddShader syntax
	fly_shaderProgram.Link();

	fly_shaderProgram.DeclareUniform("Texture");
	fly_shaderProgram.DeclareUniform("ProjMtx");
	// Attributes Here, first buffers?, buffers after?...

	ImGui_ImplFlyLib_PrepareBuffers();

	fly_shaderProgram.GiveAttributesFromMesh(&fly_dataHandle);
	fly_shaderProgram.EnableOwnAttributes();

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_CreateFontsTexture()
{
	fly_backupState.BackUp();

	ImGuiIO& io = ImGui::GetIO();
	fly_fontTexture.Init(FLY_RGBA);
	io.Fonts->GetTexDataAsRGBA32((uchar**)&fly_fontTexture.pixels, &fly_fontTexture.w, &fly_fontTexture.h);

	FLY_TexAttrib* min_filter = fly_fontTexture.AddParameter();
	min_filter->Set(FLY_MIN_FILTER, FLY_INT, new int32(FLY_LINEAR));

	FLY_TexAttrib* mag_filter = fly_fontTexture.AddParameter();
	mag_filter->Set(FLY_MAG_FILTER, FLY_INT, new int32(FLY_LINEAR));

	fly_fontTexture.SetParameters();
	fly_fontTexture.SendToGPU();

	io.Fonts->TexID = (ImTextureID)(intptr_t)fly_fontTexture.id;

	fly_backupState.SetUp();
}

void ImGui_ImplFlyLib_StaticRenderState()
{
	fly_renderState.BackUp();

	fly_renderState.blend = true;
	fly_renderState.blend_equation_rgb = fly_renderState.blend_equation_alpha = FLY_FUNC_ADD;
	fly_renderState.cull_faces = false;
	fly_renderState.depth_test = false;
	fly_renderState.scissor_test = true;
	fly_renderState.active_texture_unit = FLY_TEXTURE0;
	fly_renderState.blend_func_dst_alpha = FLY_ONE_MINUS_SRC_ALPHA;
	fly_renderState.blend_func_src_alpha = FLY_SRC_ALPHA;
	fly_renderState.blend_func_dst_rgb = FLY_ONE_MINUS_SRC_ALPHA;
	fly_renderState.blend_func_src_rgb = FLY_SRC_ALPHA;
	fly_renderState.polygon_mode = int2(FLY_FILL, FLY_FILL);
	fly_renderState.texture2d = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImGui_ImplFlyLib_UpdateMousePosAndButtons()
{
	// Update Position
	// Get if window is focused and so something about it
	FLYINPUT_GetMousePos(&fly_MousePos.x, &fly_MousePos.y);

	// Update Wheel
	FLYINPUT_GetMouseWheel(&fly_MouseWheel, &fly_MouseWheelH);

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
#include <string>
extern int global_argc;
extern char** global_argv;
bool ImGui_ImplFlyLib_Init()
{
	
    fly_Time.Start();
    bool ret = true;

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
    uint16 dpi = FLYDISPLAY_GetDPIDensity();
	uint16 w, h; FLYDISPLAY_GetSize(0, &w, &h);
	uint16 bigger = (w > h) ? w : h;
	float scale = (bigger / dpi)*.8;
	//io.FontGlobalScale = scale;
	//ImGui::GetStyle().ScaleAllSizes(scale);

    // BackendFlags and things...
    //io.BackendFlags &= ~ImGuiBackendFlags_HasMouseCursors;
	//io.BackendFlags &= ~ImGuiBackendFlags_HasSetMousePos;
	io.BackendPlatformName = "imgui_impl_flylib";
	
	// Testing custom font...
	io.Fonts->Clear();
	std::string argv = global_argv[global_argc - 1];
	std::string exe = strrchr(global_argv[global_argc - 1], FOLDER_ENDING);
	size_t v = argv.find(std::string_view(exe));
	argv = argv.substr(0, argv.length() - exe.length()) + FOLDER_ENDING + std::string("Roboto-Medium.ttf");
	io.Fonts->AddFontFromFileTTF(argv.c_str(), 13.f);
	// Does not solve font rendering issue

	ImGui_ImplFlyLib_StaticRenderState();
	ImGui_ImplFlyLib_CreateFontsTexture();
	ImGui_ImplFlyLib_CreateShaderProgram();
	//... continue

	io.RenderDrawListsFn = ImGui_ImplFlyLib_RenderDrawListsFn;

	
    return ret;
}

void ImGui_ImplFlyLib_Shutdown()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;

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
	
    io.KeyCtrl  = io.KeysDown[FLY_KEY_LEFT_CTRL] || io.KeysDown[FLY_KEY_RIGHT_CTRL]; 
	io.KeyShift = io.KeysDown[FLY_KEY_LEFT_SHIFT] || io.KeysDown[FLY_KEY_RIGHT_SHIFT];
	io.KeyAlt   = io.KeysDown[FLY_KEY_LEFT_ALT] || io.KeysDown[FLY_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[FLY_KEY_LEFT_SUPER] || io.KeysDown[FLY_KEY_RIGHT_SUPER];;

	int code;
	while((code = FLYINPUT_GetCharFromBuffer()) != -1)
		io.AddInputCharacter(code);

    // More Input Updates? ...
}