#include <stdint.h>
#include <squelib.h>

#include "imgui_impl_squelib.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VAR DEFS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Timing
static SQUE_Timer    sque_Time;
static float        sque_lastTime = 0;
// Input Variables
static bool			sque_wantKeyboard = false;
static bool			sque_KeyCtrl = false;
static bool			sque_KeyShift = false;
static bool			sque_KeyAlt = false;
static bool			sque_KeySuper = false;
static ImVec2		sque_MousePos = { -FLT_MAX, -FLT_MAX };
static bool			sque_NoButtonActiveLast = false;
static bool         sque_MousePressed_last[5] = { false, false, false, false, false };
static bool         sque_MousePressed[5] = { false, false, false, false, false };
static float        sque_MouseWheel = 0.0f;
static float		sque_MouseWheelH = 0.0f;

static KeyCallback			sque_PrevKeyboardCallback;
static MouseFloatCallback	sque_PrevMousePosCallback;
static MouseFloatCallback	sque_PrevMouseScrollCallback;
static VoidFun				sque_PrevOnResumeCallback;
static VoidFun				sque_PrevOnGoBackgroundCallback;
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
static uint16_t         sque_displayWidth = 0;
static uint16_t         sque_displayHeight = 0;
static SQUE_Texture2D    sque_fontTexture;
static SQUE_RenderState  sque_backupState;
static SQUE_RenderState  sque_renderState;
static SQUE_Program      sque_shaderProgram;
static SQUE_Mesh         sque_dataHandle;
static SQUE_Shader		sque_vertShader;
static SQUE_Shader		sque_fragShader;
static void*			sque_fontPixels;


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDER FUNCTIONS //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImGui_ImplSqueLib_VariableRenderState(ImDrawData* draw_data, int32_t  fb_width, int32_t fb_height)
{
	SQUE_RENDER_SetViewport(0, 0, fb_width, fb_height);
	sque_renderState.SetUp();
	SQUE_RENDER_SetActiveTextureUnit(SQUE_TEXTURE0);

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

	SQUE_RENDER_UseProgram(sque_shaderProgram.id);
	SetInt(SQUE_PROGRAM_GetUniformLocation(sque_shaderProgram.uniform_ref, "Texture"), 0);
	SetMatrix4(SQUE_PROGRAM_GetUniformLocation(sque_shaderProgram.uniform_ref, "ProjMtx"), &ortho_projection[0][0]);
}

void ImGui_ImplSqueLib_Render(ImDrawData* draw_data)
{
	sque_backupState.BackUp();

	int32_t fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
	int32_t fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
	if (fb_width <= 0 || fb_height <= 0) return;

	ImGui_ImplSqueLib_VariableRenderState(draw_data, fb_width, fb_height);

	// Will project scissor/clipping rectangles into framebuffer space
	ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
	ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

	SQUE_RENDER_BindMeshBuffer(sque_dataHandle.vert_id, sque_dataHandle.index_id, sque_dataHandle.attribute_object);
	for (int i = 0; i < draw_data->CmdListsCount; ++i)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[i];
		SQUE_MESHES_SetVertData(sque_dataHandle, cmd_list->VtxBuffer.Size);
		SQUE_MESHES_SetIndexData(sque_dataHandle, cmd_list->IdxBuffer.Size, SQUE_USHORT);
		SQUE_RENDER_SendMeshToGPU(sque_dataHandle, cmd_list->VtxBuffer.Data, cmd_list->IdxBuffer.Data);

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
					SQUE_RENDER_Scissor((int32_t)clip_rect.x, (int32_t)(fb_height - clip_rect.w), (int32_t)(clip_rect.z - clip_rect.x), (int32_t)(clip_rect.w - clip_rect.y));
					SQUE_RENDER_BindExternalTexture(SQUE_TEXTURE_2D, (uint32_t)(intptr_t)pcmd->TextureId);
					SQUE_RENDER_DrawIndices(sque_dataHandle, pcmd->IdxOffset, pcmd->ElemCount);
				}
			}
		}

	}
	//sque_dataHandle.ChangeVertData(0, NULL);
	//sque_dataHandle.ChangeIndexData(0, NULL);

	sque_backupState.SetUp();
}

void ImGui_ImplSqueLib_PrepareBuffers()
{
	sque_backupState.BackUp();

	SQUE_RENDER_GenerateMeshBuffer(&sque_dataHandle.vert_id, &sque_dataHandle.index_id, &sque_dataHandle.attribute_object);
	SQUE_MESHES_SetDrawConfig(sque_dataHandle, SQUE_TRIANGLES, SQUE_STREAM_DRAW);

	SQUE_MESHES_DeclareAttributes(sque_dataHandle.vert_id, sque_dataHandle.attrib_ref, 3);
	SQUE_MESHES_AddAttribute(sque_dataHandle.attrib_ref, SQUE_VertAttrib("Position", SQUE_FLOAT, false, 2));
	SQUE_MESHES_AddAttribute(sque_dataHandle.attrib_ref, SQUE_VertAttrib("UV", SQUE_FLOAT, false, 2));
	SQUE_MESHES_AddAttribute(sque_dataHandle.attrib_ref, SQUE_VertAttrib("Color", SQUE_UBYTE, true, 4));
	SQUE_RENDER_BindMeshBuffer(sque_dataHandle.vert_id, sque_dataHandle.index_id, sque_dataHandle.attribute_object);
	SQUE_MESHES_SetLocations(sque_dataHandle.attrib_ref);

	sque_backupState.SetUp();
}

static char* easy_concat(const char* s1, const char* s2)
{
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char* result = new char[len1 + len2 + 1]; // +1 for the null-terminator
	// in real code you would check for errors in malloc here
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
	return result;
}

void ImGui_ImplSqueLib_CreateShaderProgram()
{
	sque_backupState.BackUp();

	const char* glsl = SQUE_RENDER_GetGLSLVer();

	SQUE_SHADERS_Generate(sque_vertShader, SQUE_VERTEX_SHADER);
	std::string vert(easy_concat(glsl, vertex_shader));
	SQUE_SHADERS_SetSource(sque_vertShader.id, vert.c_str());
	SQUE_SHADERS_Compile(sque_vertShader.id);

	SQUE_SHADERS_Generate(sque_fragShader, SQUE_FRAGMENT_SHADER);
	std::string frag(easy_concat(glsl, fragment_shader));
	SQUE_SHADERS_SetSource(sque_fragShader.id, frag.c_str());
	SQUE_SHADERS_Compile(sque_fragShader.id);

	SQUE_RENDER_CreateProgram(&sque_shaderProgram.id);
	SQUE_PROGRAM_AttachShader(sque_shaderProgram, sque_vertShader.id, sque_vertShader.type);
	SQUE_PROGRAM_AttachShader(sque_shaderProgram, sque_fragShader.id, sque_fragShader.type);

	SQUE_RENDER_LinkProgram(sque_shaderProgram.id);
	
	SQUE_SHADERS_DeclareProgram(sque_shaderProgram.uniform_ref, sque_shaderProgram.id, 2);
	SQUE_SHADERS_DeclareUniform(sque_shaderProgram.uniform_ref, sque_shaderProgram.id, "Texture");
	SQUE_SHADERS_DeclareUniform(sque_shaderProgram.uniform_ref, sque_shaderProgram.id, "ProjMtx");

	ImGui_ImplSqueLib_PrepareBuffers();

	sque_backupState.SetUp();
}

void ImGui_ImplSqueLib_CreateFontsTexture()
{
	sque_backupState.BackUp();

	ImGuiIO& io = ImGui::GetIO();
	SQUE_RENDER_GenTextureData(&sque_fontTexture.id);
	sque_fontTexture.var_type = SQUE_UBYTE;
	sque_fontTexture.var_size = 1;
	sque_fontTexture.data_format = SQUE_RGBA;
	sque_fontTexture.use_format = SQUE_RGBA;
	io.Fonts->GetTexDataAsRGBA32((uchar**)&sque_fontPixels, &sque_fontTexture.w, &sque_fontTexture.h);

	int32_t filter = SQUE_LINEAR;
	SQUE_TEXTURES_DeclareIntAttributes(sque_fontTexture.id, 2);
	SQUE_TEXTURES_AddIntAttribute(sque_fontTexture.id, SQUE_TexAttrib(SQUE_MIN_FILTER, &filter));
	SQUE_TEXTURES_AddIntAttribute(sque_fontTexture.id, SQUE_TexAttrib(SQUE_MAG_FILTER, &filter));
	SQUE_RENDER_BindTex2D(sque_fontTexture.id);
	SQUE_RENDER_SetTextureAttributes(sque_fontTexture.id);
	SQUE_RENDER_SendTex2DToGPU(sque_fontTexture, sque_fontPixels);

	io.Fonts->TexID = (ImTextureID)(intptr_t)sque_fontTexture.id;

	sque_backupState.SetUp();
}

void ImGui_ImplSqueLib_StaticRenderState()
{
	sque_backupState.BackUp();

	sque_renderState.BackUp();

	sque_renderState.blend = true;
	sque_renderState.blend_equation_rgb = SQUE_FUNC_ADD;
	sque_renderState.blend_equation_alpha = SQUE_FUNC_ADD;
	sque_renderState.cull_faces = false;
	sque_renderState.depth_test = false;
	sque_renderState.scissor_test = true;
	sque_renderState.blend_func_dst_alpha = SQUE_ONE;
	sque_renderState.blend_func_src_alpha = SQUE_ONE;
	sque_renderState.blend_func_dst_rgb = SQUE_ONE_MINUS_SRC_ALPHA;
	sque_renderState.blend_func_src_rgb = SQUE_SRC_ALPHA;
	sque_renderState.blend_func_separate = true;
	sque_renderState.polygon_mode[0] = SQUE_FILL;
	sque_renderState.polygon_mode[1] = SQUE_FILL;

	sque_backupState.SetUp();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INPUT FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void ImGui_ImplSqueLib_MousePosCallback(float x, float y)
{
	sque_MousePos.x = x;
	sque_MousePos.y = y;
	sque_PrevMousePosCallback(x, y);
}
void ImGui_ImplSqueLib_MouseScrollCallback(float x, float y)
{
	sque_MouseWheel = y;
	sque_MouseWheelH = x;
	sque_PrevMouseScrollCallback(x, y);
}

void ImGui_ImplSqueLib_UpdateMouseButtons()
{

	// Update Mouse / Pointer / MouseButtons / ScrollWheel
	for(int i = 0; i < IM_ARRAYSIZE(sque_MousePressed); ++i)
	{
		SQUE_INPUT_Actions state = SQUE_INPUT_GetMouseButton(i);
		sque_MousePressed_last[i] = sque_MousePressed[i];
		sque_MousePressed[i] = !(state == SQUE_ACTION_RELEASE || state == SQUE_ACTION_UNKNOWN);
	}
}

void ImGui_ImplSqueLib_KeyboardCallback(int key, int state)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[key] = (state > SQUE_RELEASED) ? true : false;
	sque_PrevKeyboardCallback(key, state);
}

const char* ImGui_ImplSqueLib_GetClipboardText()
{
    return "TODO...";
}

void ImGui_ImplSqueLib_SetClipboardText(const char* text)
{

}

void ImGui_ImplSqueLib_Resume()
{
	ImGui_ImplSqueLib_Init();
	if (sque_PrevOnResumeCallback != NULL) sque_PrevOnResumeCallback();
}

void ImGui_ImplSqueLib_GoBackground()
{

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;
	//sque_shaderProgram.CleanUp();

	//sque_dataHandle.CleanUp();
	delete sque_fontPixels;

	if (sque_PrevOnGoBackgroundCallback != NULL) sque_PrevOnGoBackgroundCallback();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PUBLIC USAGE FUNCTIONS ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Has Errors

bool init_registered = false;
void ImGui_ImplSqueLib_Init()
{
	sque_Time.Start();

	if(!init_registered)
	{
		sque_PrevMousePosCallback = SQUE_INPUT_SetPointerPosCallback(ImGui_ImplSqueLib_MousePosCallback, 0);
		sque_PrevMouseScrollCallback = SQUE_INPUT_SetScrollCallback(ImGui_ImplSqueLib_MouseScrollCallback);
		sque_PrevKeyboardCallback = SQUE_INPUT_SetKeyCallback(ImGui_ImplSqueLib_KeyboardCallback);
		sque_PrevOnResumeCallback = SQUE_AddOnResumeCallback(ImGui_ImplSqueLib_Resume);
		sque_PrevOnGoBackgroundCallback = SQUE_AddOnGoBackgroundCallback(ImGui_ImplSqueLib_GoBackground);
	}
	
    ImGuiIO &io = ImGui::GetIO();

    // Init KeyMap
    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_Tab] = SQUE_KEY_TAB;                     
	io.KeyMap[ImGuiKey_LeftArrow] = SQUE_KEY_ARROW_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SQUE_KEY_ARROW_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SQUE_KEY_ARROW_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SQUE_KEY_ARROW_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SQUE_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = SQUE_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = SQUE_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = SQUE_KEY_END;
	io.KeyMap[ImGuiKey_Delete] = SQUE_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SQUE_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SQUE_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = SQUE_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SQUE_KEY_UPPER_A;
	io.KeyMap[ImGuiKey_C] = SQUE_KEY_UPPER_C;
	io.KeyMap[ImGuiKey_V] = SQUE_KEY_UPPER_V;
	io.KeyMap[ImGuiKey_X] = SQUE_KEY_UPPER_X;
	io.KeyMap[ImGuiKey_Y] = SQUE_KEY_UPPER_Y;
	io.KeyMap[ImGuiKey_Z] = SQUE_KEY_UPPER_Z;

    // BackendFlags and things...
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	//io.BackendFlags &= ~ImGuiBackendFlags_HasSetMousePos;
	io.BackendPlatformName = "imgui_impl_squelib";
	
	// how will it work with multiple windows and multiple viewports?
	// Do the multiwindows have to share the context?
	// Does ImGui have to be initialized on a per window basis?
	//io.ImeWindowHandle = SQUE_DISPLAY_GetPlatformWindowHandle(0);
	
	
	// Initialize Render State...
	ImGui_ImplSqueLib_StaticRenderState();
	ImGui_ImplSqueLib_CreateFontsTexture();
	ImGui_ImplSqueLib_CreateShaderProgram();

	if(init_registered && sque_PrevOnResumeCallback != NULL) sque_PrevOnResumeCallback();
	init_registered = true;
}

void ImGui_ImplSqueLib_Shutdown()
{
	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->TexID = 0;

	delete sque_fontPixels;

	SQUE_INPUT_SetPointerPosCallback(sque_PrevMousePosCallback, 0);
	SQUE_INPUT_SetScrollCallback(sque_PrevMouseScrollCallback);
	SQUE_INPUT_SetKeyCallback(sque_PrevKeyboardCallback);
	SQUE_AddOnResumeCallback(sque_PrevOnResumeCallback);
	SQUE_AddOnGoBackgroundCallback(sque_PrevOnGoBackgroundCallback);

	init_registered = false;
}

void ImGui_ImplSqueLib_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

	if(sque_wantKeyboard != io.WantTextInput) 
	{
		SQUE_INPUT_DisplaySoftwareKeyboard(io.WantTextInput);
		sque_wantKeyboard = io.WantTextInput;
	}
	IM_ASSERT(io.Fonts->IsBuilt());

	int32_t w, h;
	SQUE_DISPLAY_GetWindowSize(0, &w, &h); 
	w = (w <= 0) ? 1 : w;
	h = (h <= 0) ? 1 : h;
	io.DisplaySize = ImVec2(w, h);
	SQUE_DISPLAY_GetViewportSize(0, &w, &h);
	//SQUE_PRINT(LT_INFO, "%d,%d %d,%d", w,h, io.DisplaySize.x, io.DisplaySize.y);
	io.DisplayFramebufferScale = ImVec2(((float)w/ io.DisplaySize.x), ( (float)h / io.DisplaySize.y));

    // Setup Time Step
    double curr_time = (sque_Time.ReadMilliSec() / 1000);
    io.DeltaTime = std::abs(curr_time - sque_lastTime)+0.000001;
	//SQUE_PRINT(LT_INFO, "%f %f %f", io.DeltaTime, sque_lastTime, curr_time);
    sque_lastTime = curr_time;

    // Update Mouse
	ImGui_ImplSqueLib_UpdateMouseButtons();

    for(int i = 0; i < IM_ARRAYSIZE(io.MouseDown); ++i)
		io.MouseDown[i] = sque_MousePressed[i];

	
	io.MousePos = sque_MousePos;
	
    io.MouseWheel   = sque_MouseWheel;
	io.MouseWheelH  = sque_MouseWheelH;
	sque_MouseWheel = sque_MouseWheelH = 0;

    io.KeyCtrl  = io.KeysDown[SQUE_KEY_LEFT_CTRL] || io.KeysDown[SQUE_KEY_RIGHT_CTRL]; 
	io.KeyShift = io.KeysDown[SQUE_KEY_LEFT_SHIFT] || io.KeysDown[SQUE_KEY_RIGHT_SHIFT];
	io.KeyAlt   = io.KeysDown[SQUE_KEY_LEFT_ALT] || io.KeysDown[SQUE_KEY_RIGHT_ALT];
	io.KeySuper = io.KeysDown[SQUE_KEY_LEFT_SUPER] || io.KeysDown[SQUE_KEY_RIGHT_SUPER];;

	int code;
	while((code = SQUE_INPUT_GetCharFromBuffer()) != -1)
		io.AddInputCharacter(code);

    // More Input Updates? ...
}