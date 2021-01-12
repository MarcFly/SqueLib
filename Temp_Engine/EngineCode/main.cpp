#include <iostream>
#include <fly_lib.h>
//#include <imgui.h>
//#include <imgui_impl_flylib.h>

enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

#include <cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void LearnOpenGLTest()
{
    // Shader Setup
    const char* vertexShaderSource =
        "layout (location = 0) in vec3 aPos;\n"
        "layout (location = 1) in vec3 aCol;\n"
        "layout (location = 2) in vec2 aTexCoord;\n"
        "out vec3 v_col;\n"
        "out vec2 v_uv;\n"
        "void main()\n"
        "{\n"
        "   v_uv = aTexCoord;\n"
        "   v_col = aCol;\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char* vert_source[2] = { FLYRENDER_GetGLSLVer(), vertexShaderSource };
    FLY_Shader* vert_s = FLYSHADER_Create(FLY_VERTEX_SHADER, 2, vert_source, true);
    vert_s->Compile();

    const char* fragmentShaderSource =
        "out vec4 FragColor;\n"
        "in vec2 v_uv;\n"
        "in vec3 v_col;\n"
        "uniform vec4 ourColor;\n"
        "uniform sampler2D ourTexture;\n"
        "void main() { FragColor = ourColor+vec4(v_col, 1.0)+texture(ourTexture, v_uv);}\0";
    const char* frag_source[2] = { FLYRENDER_GetGLSLVer(), fragmentShaderSource };
    FLY_Shader* frag_s = FLYSHADER_Create(FLY_FRAGMENT_SHADER, 2, frag_source, true);
    frag_s->Compile();

    FLY_Program program;
    program.Init();
    program.AttachShader(&vert_s);
    program.AttachShader(&frag_s);
    program.Link();

    FLY_CHECK_RENDER_ERRORS();
    // Texture Setup--------------------------------------------------------------------------------
    FLY_Texture2D tex;
    tex.Init(FLY_RGB);
    FLY_TexAttrib* wrap_s = tex.AddParameter();
    wrap_s->Set(FLY_WRAP_S, FLY_INT, new int32(FLY_MIRROR));
    FLY_TexAttrib* wrap_t = tex.AddParameter();
    wrap_t->Set(FLY_WRAP_T, FLY_INT, new int32(FLY_MIRROR));
    //FLY_TexAttrib* border_color = tex.AddParameter();
    //border_color->Set(FLY_BORDER_COLOR,FLY_FLOAT, new float4(1.0f, 1.0f, 0.0f, 1.0f));

    FLY_TexAttrib* min_filter = tex.AddParameter();
    min_filter->Set(FLY_MIN_FILTER, FLY_INT, new int32(FLY_NEAREST));
    FLY_TexAttrib* mag_filter = tex.AddParameter();
    mag_filter->Set(FLY_MAG_FILTER, FLY_INT, new int32(FLY_LINEAR));
    tex.SetParameters();

    tex.pixels = stbi_load("C:/Users/MarcFly/Documents/Repos/TFG-TempEngine/Temp_Engine/builds/Windows/Debug/container.jpg", &tex.w, &tex.h, &tex.channel_num, 0);
    tex.SendToGPU();
    stbi_image_free(tex.pixels);

    // Mesh Setup-----------------------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    FLY_Mesh triangle;
    triangle.verts = (char*)vertices;
    triangle.num_verts = 4;
    triangle.SetDrawMode(FLY_STATIC_DRAW);
    triangle.indices = (char*)indices;
    triangle.SetIndexVarType(FLY_UINT);
    triangle.num_index = 6;

    FLY_VertAttrib* p = triangle.AddAttribute();
    p->SetNumComponents(3);
    p->SetVarType(FLY_FLOAT);
    p->SetNormalize(false);

    FLY_VertAttrib* c = triangle.AddAttribute();
    c->SetNumComponents(3);
    c->SetVarType(FLY_FLOAT);
    c->SetNormalize(false);

    FLY_VertAttrib* uv = triangle.AddAttribute();
    uv->SetNumComponents(2);
    uv->SetVarType(FLY_FLOAT);
    uv->SetNormalize(false);

    triangle.SetOffsetsInOrder();
    triangle.Init();
    triangle.SendToGPU();
    triangle.SetLocationsInOrder();


    program.Use();
    program.DeclareUniform("ourColor");

    ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
    FLY_Timer t;
    bool window = true;
    while (!FLYDISPLAY_ShouldWindowClose(0))
    {
        FLYRENDER_Clear(NULL, &col);
        program.Use();
        float green = sin(t.ReadMilliSec() / 200.f) + 0.5f;
        program.SetFloat4("ourColor", float4(0, green, 0, 1));
        tex.BindToUnit(FLY_TEXTURE0);
        program.DrawIndices(&triangle);

        FLYDISPLAY_SwapAllBuffers();
        FLYINPUT_Process(0);
    }
    triangle.verts = NULL;
    triangle.indices = NULL;
}

#include <imgui.h>
#include <imgui_impl_flylib.h>
void ImGuiImplFlyLibTest()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplFlyLib_Init();
    ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
    bool show_demo_window = true;
    while(!FLYDISPLAY_ShouldWindowClose(0))
    {
        FLYRENDER_Clear(NULL, &col);
        ImGui_ImplFlyLib_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow(&show_demo_window);
        ImGui::Render();
        FLYDISPLAY_SwapAllBuffers();
        FLYINPUT_Process(0);
    }

    ImGui_ImplFlyLib_Shutdown();
}
int global_argc;
char** global_argv;
int main(int argc, char**argv)
{
    global_argc = argc;
    global_argv = argv;
    main_states state = MAIN_CREATION;
    bool ret = true;

    // Helpers Initialization
    {
        ret = FLYLIB_Init(/*flags*/);
    }

    // Engine Initialization
    {
        FLYLOG(FLY_LogType::LT_INFO, "Initializing Engine...");
        // Initialize all modules in required order

        state = (ret) ? MAIN_UPDATE : MAIN_EXIT;

        if (ret) { FLYLOG(FLY_LogType::LT_INFO, "Entering Update Loop..."); }
        else FLYLOG(FLY_LogType::LT_WARNING, "Error Initializing Engine...");
    }

    //LearnOpenGLTest();
    FLYPRINT(LT_INFO, "Testing Crash...");
    ImGuiImplFlyLibTest();
    // For Testing timer and android keyboard
    
    FLYINPUT_DisplaySoftwareKeyboard(true);
    while(state == MAIN_UPDATE)
    {
        if(FLYDISPLAY_ShouldWindowClose(0))
            state = MAIN_FINISH;
    }

    //  Engine CleanUp
    {
        FLYLOG(FLY_LogType::LT_INFO, "Cleaning Up Engine...");

        // perform the CleanUp of all modules in reverse init order preferably

        if (ret) { FLYLOG(FLY_LogType::LT_INFO, "Application Cleanup Performed Successfully..."); }
        else FLYLOG(FLY_LogType::LT_WARNING, "Application Cleanup with errors...");

        // Delete memory bound modules
    }

    // Helpers CleanUp
    {
        FLYLOG(FLY_LogType::LT_INFO, "Finishing Executing Engine...");
        // Close something that is not part of the engine as a module

        FLYLOG(FLY_LogType::LT_INFO, "Closing Helpers...");
        FLYLIB_Close();
    }

    return 0;
}