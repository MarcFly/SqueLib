#include <iostream>
#include <fly_lib.h>

enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#define STB_NEON
#include <stb_image.h>

#include <imgui.h>
#include <imgui_impl_flylib.h>

bool have_resumed;

void ImGuiImplFlyLibTest()
{
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
    FLY_Shader* vert_s = new FLY_Shader(FLY_VERTEX_SHADER, 2, vert_source);
    vert_s->Compile();

    const char* fragmentShaderSource =
        "out vec4 FragColor;\n"
        "in vec2 v_uv;\n"
        "in vec3 v_col;\n"
        "uniform vec4 ourColor;\n"
        "uniform sampler2D ourTexture;\n"
        "void main() { FragColor = ourColor+vec4(v_col, 1.0)+texture(ourTexture, v_uv);}\0";
    const char* frag_source[2] = { FLYRENDER_GetGLSLVer(), fragmentShaderSource };
    FLY_Shader* frag_s = new FLY_Shader(FLY_FRAGMENT_SHADER, 2, frag_source);
    frag_s->Compile();

    FLY_Program program;
    FLYRENDER_CreateProgram(&program);
    program.AttachShader(vert_s);
    program.AttachShader(frag_s);
    FLYRENDER_LinkProgram(program);
        
    FLY_CHECK_RENDER_ERRORS();
    // Texture Setup--------------------------------------------------------------------------------
    FLY_Texture2D tex;
    FLY_GenTextureData(&tex);
    tex.format = FLY_RGB;
    tex.var_type = FLY_UBYTE;
    FLY_TexAttrib* wrap_s = new FLY_TexAttrib(FLY_WRAP_S, FLY_INT, new int32_t(FLY_MIRROR));
    tex.SetParameter(wrap_s);

    FLY_TexAttrib* wrap_t = new FLY_TexAttrib(FLY_WRAP_T, FLY_INT, new int32_t(FLY_MIRROR));
    tex.SetParameter(wrap_t);

    FLY_TexAttrib* min_filter = new FLY_TexAttrib(FLY_MIN_FILTER, FLY_INT, new int32_t(FLY_LINEAR));
    tex.SetParameter(min_filter);
    FLY_TexAttrib* mag_filter = new FLY_TexAttrib(FLY_MAG_FILTER, FLY_INT, new int32_t(FLY_LINEAR));
    tex.SetParameter(mag_filter);
    tex.ApplyParameters();

    FLY_Asset* tex_data = FLYFS_GetAssetRaw(NULL, "container.jpeg");
    tex.pixels = stbi_load_from_memory((unsigned char*)tex_data->raw_data, tex_data->size, &tex.w, &tex.h, &tex.channel_num, 0);
    FLY_SendTextureToGPU(tex);
    stbi_image_free(tex.pixels);

    FLY_CHECK_RENDER_ERRORS();

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
    triangle.draw_config = FLY_TRIANGLES;
    triangle.draw_mode = FLY_STATIC_DRAW;
    triangle.indices = (char*)indices;
    triangle.index_var = FLY_UINT;
    triangle.index_var_size = 4;
    triangle.num_index = 6;

    triangle.AddAttribute(new FLY_VertAttrib("aPos", FLY_FLOAT, false, 3));
    triangle.AddAttribute(new FLY_VertAttrib("aCol", FLY_FLOAT, false, 3));
    triangle.AddAttribute(new FLY_VertAttrib("aTexCoord", FLY_FLOAT, false, 2));

    FLY_GenerateMeshBuffer(&triangle);
    FLY_BindMeshBuffer(triangle);
    FLY_SendMeshToGPU(triangle);
    triangle.SetLocationsInOrder();

    FLYRENDER_UseProgram(program);
    program.DeclareUniform("ourColor");

    ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
    FLY_Timer t;

    FLY_CHECK_RENDER_ERRORS();
    //-------------------------------------------------------------------

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplFlyLib_Init();
    //ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
    bool show_demo_window = true;
    bool show_another_window = true;
    
    while(!FLYDISPLAY_ShouldWindowClose(0))
    {

if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        FLYINPUT_Process(0);
        if(have_resumed) FLY_CHECK_RENDER_ERRORS();

        ImGui_ImplFlyLib_NewFrame();
        if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        ImGui::NewFrame();

        if(show_demo_window)ImGui::ShowDemoWindow(&show_demo_window);
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&col); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();

        int32_t x, y;
        FLYDISPLAY_GetViewportSize(0, &x, &y);
if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        // Remember to make the expected framebuffer (aka window) to main
    FLYDISPLAY_MakeContextMain(0);
        FLYRENDER_ChangeFramebufferSize(x, y);
if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        FLYRENDER_Clear(col);
        //--
        FLYRENDER_UseProgram(program);
        if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        float green = sin(t.ReadMilliSec() / 200.f) + 0.5f;
        SetFloat4(program, "ourColor", glm::vec4(0, green, 0, 1));

        FLY_SetActiveTextureUnit(FLY_TEXTURE0);
        FLY_BindTexture(tex);

        FLYRENDER_DrawIndices(triangle);
        if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        
        //--
if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        ImGui_ImplFlyLib_Render(ImGui::GetDrawData());
if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        FLYDISPLAY_SwapAllBuffers();
if(have_resumed) FLY_CHECK_RENDER_ERRORS();
        
        
    }

    //----------------------
    triangle.verts = NULL;
    triangle.indices = NULL;
    //----------------------
    ImGui_ImplFlyLib_Shutdown();
}

int main(int argc, char**argv)
{
    main_states state = MAIN_CREATION;
    bool ret = true;

    // Helpers Initialization
    {
        FLYLIB_Init("Flylib Testing Grounds", NULL);
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
    ImGuiImplFlyLibTest();
    // For Testing timer and android keyboard
    
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