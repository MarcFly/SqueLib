#include <squelib.h>
#include <imgui.h>
#include <imgui_impl_squelib.h>

const char* vert_shader =
"layout (location = 0) in vec2 vertPos;\n"
"uniform vec2 center;\n"
"uniform vec2 size;\n"
"uniform vec2 vp;\n"
"out vec2 c;\n"
"void main()\n"
"{\n"
"vec2 ndc_pos = 2.*((center / vp) - vec2(.5));"
"vec2 ndc_size = ( (vertPos-ndc_pos) / abs(vertPos-ndc_pos) ) * (size / vp);"
"c = center;"
"gl_Position = vec4(ndc_pos+ndc_size, 0,1);"
"}\0";

const char* frag_shader =
"out vec4 FragColor;\n"
"in vec2 c;\n"
"uniform float radius;\n"
"uniform vec3 col_in;\n"
"void main(){\n"
"float col = step(length(gl_FragCoord.xy-c), radius);\n"
"FragColor = vec4(col_in,col);\n"
"}\0";

static bool on_background = false;

void OnResume()
{
    on_background = false;
}

void OnGoBackground()
{
    on_background = true;
}

static float vertices[] = {
    1,  1,
    1, -1,
    -1, -1,
    -1,  1
};

static uint32_t indices[]{
    0,1,3,
    1,2,3
};

static SQUE_RenderState state;
static SQUE_Program program;
static SQUE_Mesh quad;
void InitShader()
{
    {
        state.BackUp();
        state.blend = true;
        state.blend_func_dst_alpha = SQUE_ONE_MINUS_SRC_ALPHA;
        state.blend_func_src_alpha = SQUE_SRC_ALPHA;
        state.SetUp();
    }

    SQUE_MESH_SetDataConfig(&quad, 4, 6, SQUE_UINT);
    SQUE_MESH_SetDrawConfig(&quad, SQUE_TRIANGLES, SQUE_STATIC_DRAW);


    SQUE_MESH_GenBuffer(&quad);
    SQUE_MESH_BindBuffer(quad);
    SQUE_MESH_AddAttribute(&quad, "vertPos", SQUE_FLOAT, false, 2);
    SQUE_MESH_InterleaveOffsets(&quad);
    SQUE_MESH_SetLocations(&quad);
    SQUE_MESH_SendToGPU(quad, vertices, indices);

    SQUE_PROGRAM_GenerateID(&program.id);
    
    SQUE_Shader vert;
    SQUE_SHADERS_GenerateID(&vert, SQUE_VERTEX_SHADER);
    SQUE_SHADERS_SetSource(vert.id, vert_shader);
    
    SQUE_Shader frag;
    SQUE_SHADERS_GenerateID(&frag, SQUE_FRAGMENT_SHADER);
    SQUE_SHADERS_SetSource(frag.id, frag_shader);

    SQUE_SHADERS_Compile(vert.id);
    SQUE_SHADERS_Compile(frag.id);

    SQUE_PROGRAM_AttachShader(&program, vert);
    SQUE_PROGRAM_AttachShader(&program, frag);
    SQUE_PROGRAM_Link(program.id);

    SQUE_PROGRAM_Use(program.id);
    SQUE_PROGRAM_CacheUniforms(&program);
}

static float size[2] = { 50,50 };
static int32_t vp[2];
static float color[3] = { 1,0,0 };
static ColorRGBA clear_col = { 0, .3, 1., 1};

static SQUE_Timer timer;
static sque_vec<char> test_vec;
void RenderKeys()
{
    ImGui_ImplSqueLib_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowSize(ImVec2(vp[0], vp[1] + 20));
    ImGui::SetNextWindowPos(ImVec2(0, -20));
    ImGui::Begin("UI_Window", NULL, ImGuiWindowFlags_NoDecoration);
    {
        static std::string test;
        if (timer.ReadMilliSec() > 1000)
        {
            test.clear();
            timer.Start();
        }

        ImGuiIO& io = ImGui::GetIO();
        char get = (io.InputQueueCharacters.size() > 0) ? io.InputQueueCharacters.front() : -1;
        if(get != -1) 
            test.push_back(get);
        ImGui::Text(test.c_str());

        static bool software_keyboard = false;
        software_keyboard = ImGui::Button("Android Software Keyboard");
        SQUE_INPUT_DisplaySoftwareKeyboard(software_keyboard);
        
    }
    ImGui::End();
    ImGui::Render();
    ImGui_ImplSqueLib_Render(ImGui::GetDrawData());
}

void RenderPointers()
{
    state.SetUp();
    static float vpf[2];
    vpf[0] = vp[0];
    vpf[1] = vp[1];
    SQUE_PROGRAM_Use(program.id);
    SetFloat(SQUE_PROGRAM_GetUniformID(program, "radius"), size[0] / 2.f);
    SetFloat2(SQUE_PROGRAM_GetUniformID(program, "size"), size);
    SetFloat2(SQUE_PROGRAM_GetUniformID(program, "vp"), vpf);
    SetFloat3(SQUE_PROGRAM_GetUniformID(program, "col_in"), color);
    for (uint16_t i = 0; i < MAX_POINTERS; ++i)
    {
        static float pos[2];
        if (SQUE_INPUT_GetPointerPos(&pos[0], &pos[1], i))
        {
            pos[1] = vp[1] - pos[1];
            SetFloat2(SQUE_PROGRAM_GetUniformID(program, "center"), pos);
            SQUE_RENDER_DrawIndices(quad);
        }
    }
}

int main(int argc, char** argv)
{
    SQUE_LIB_Init("InputViz");
    InitShader();

    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplSqueLib_Init();
    // Scale ui appropiately...
    ImGui::GetIO().FontGlobalScale *= SQUE_DISPLAY_GetDPIDensity() / 72;
    ImGui::GetStyle().ScaleAllSizes(SQUE_DISPLAY_GetDPIDensity() / 100);

    while (!SQUE_DISPLAY_ShouldWindowClose())
    {
        SQUE_INPUT_Process(0);

        if (on_background)
        {
            SQUE_Sleep(100);
            continue;
        }
        SQUE_RENDER_Clear(clear_col);
        SQUE_DISPLAY_GetWindowSize(&vp[0], &vp[1]);

        RenderKeys();
        
        RenderPointers();
        
        SQUE_DISPLAY_SwapAllBuffers();
    }

    SQUE_LIB_Close();

    return 0;
}