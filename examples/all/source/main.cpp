#include <iostream>
#include <squelib.h>

enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

#include <cmath>

#include <imgui.h>
#include <imgui_impl_squelib.h>

bool have_resumed;

void ImGuiImplSqueLibTest()
{
    // SQUE_DISPLAY_OpenWindow("Test2ndWindow", 200,200);
    // SQUE_DISPLAY_MakeContextMain(0);
    // Currently not getting render on Window2

    SQUE_FS_CreateDirRelative("../../TestPermissions");
    // does not exist SQUE_AskPermissions("permission.ACCESS");
    SQUE_AskPermissions("INTERNET");
    SQUE_AskPermissions("HIDE_NON_SYSTEM_OVERLAY_WINDOWS");
    SQUE_AskPermissions("ACCESS_NETWORK_STATE");
    SQUE_AskPermissions("WRITE_EXTERNAL_STORAGE");
    SQUE_AskPermissions("READ_EXTERNAL_STORAGE");
    SQUE_AskPermissions("READ_PHONE_STATE");
    SQUE_AskPermissions("GET_TASKS");
    SQUE_AskPermissions("REORDER_TASKS");
    SQUE_AskPermissions("WRITE_APN_SETTINGS");
    // does not exist SQUE_AskPermissions("READ_SECURE_SETTINGS");
    // does not exist SQUE_AskPermissions("READ_SETTINGS");
    SQUE_AskPermissions("REAL_GET_TASKS");
    SQUE_AskPermissions("INTERACT_ACROSS_USERS");
    SQUE_AskPermissions("MANAGE_USERS");
    SQUE_AskPermissions("INSTALL_PACKAGES");
    SQUE_AskPermissions("DELETE_PACKAGES");
    SQUE_AskPermissions("INTERACT_ACROSS_USERS_FULL");


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
    const char* vert_source[2] = { SQUE_RENDER_GetGLSLVer(), vertexShaderSource };
    SQUE_Shader* vert_s = new SQUE_Shader(SQUE_VERTEX_SHADER, 2, vert_source);
    vert_s->Compile();

    const char* fragmentShaderSource =
        "out vec4 FragColor;\n"
        "in vec2 v_uv;\n"
        "in vec3 v_col;\n"
        "uniform vec4 ourColor;\n"
        "uniform sampler2D ourTexture;\n"
        "void main() { FragColor = ourColor+vec4(v_col, 1.0)+texture(ourTexture, v_uv);}\0";
    const char* frag_source[2] = { SQUE_RENDER_GetGLSLVer(), fragmentShaderSource };
    SQUE_Shader* frag_s = new SQUE_Shader(SQUE_FRAGMENT_SHADER, 2, frag_source);
    frag_s->Compile();

    SQUE_Program program;
    SQUE_RENDER_CreateProgram(&program);
    program.AttachShader(vert_s);
    program.AttachShader(frag_s);
    SQUE_RENDER_LinkProgram(program);
        
    SQUE_CHECK_RENDER_ERRORS();
    // Texture Setup--------------------------------------------------------------------------------
    SQUE_Texture2D tex;
    SQUE_GenTextureData(&tex);
    tex.format = SQUE_RGB;
    tex.var_type = SQUE_UBYTE;
    SQUE_TexAttrib* wrap_s = new SQUE_TexAttrib(SQUE_WRAP_S, SQUE_INT, new int32_t(SQUE_MIRROR));
    tex.SetParameter(wrap_s);

    SQUE_TexAttrib* wrap_t = new SQUE_TexAttrib(SQUE_WRAP_T, SQUE_INT, new int32_t(SQUE_MIRROR));
    tex.SetParameter(wrap_t);

    SQUE_TexAttrib* min_filter = new SQUE_TexAttrib(SQUE_MIN_FILTER, SQUE_INT, new int32_t(SQUE_LINEAR));
    tex.SetParameter(min_filter);
    SQUE_TexAttrib* mag_filter = new SQUE_TexAttrib(SQUE_MAG_FILTER, SQUE_INT, new int32_t(SQUE_LINEAR));
    tex.SetParameter(mag_filter);
    tex.ApplyParameters();

    SQUE_Asset* tex_data = SQUE_FS_GetAssetRaw(NULL, "container.jpeg");
    if (!SQUE_LOAD_Texture(tex_data, &tex))
        SQUE_PRINT(LT_WARNING, "Texture not loaded...");

    SQUE_CHECK_RENDER_ERRORS();

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
    SQUE_Mesh triangle;

    triangle.verts = (char*)vertices;
    triangle.num_verts = 4;
    triangle.draw_config = SQUE_TRIANGLES;
    triangle.draw_mode = SQUE_STATIC_DRAW;
    triangle.indices = (char*)indices;
    triangle.index_var = SQUE_UINT;
    triangle.index_var_size = 4;
    triangle.num_index = 6;

    triangle.AddAttribute(new SQUE_VertAttrib("aPos", SQUE_FLOAT, false, 3));
    triangle.AddAttribute(new SQUE_VertAttrib("aCol", SQUE_FLOAT, false, 3));
    triangle.AddAttribute(new SQUE_VertAttrib("aTexCoord", SQUE_FLOAT, false, 2));

    SQUE_GenerateMeshBuffer(&triangle);
    SQUE_BindMeshBuffer(triangle);
    SQUE_SendMeshToGPU(triangle);
    triangle.SetLocationsInOrder();

    SQUE_RENDER_UseProgram(program);
    program.DeclareUniform("ourColor");

    ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
    SQUE_Timer t;

    SQUE_CHECK_RENDER_ERRORS();
    //-------------------------------------------------------------------
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();
    ImGui_ImplSqueLib_Init();
    //ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
    bool show_demo_window = true;
    bool show_another_window = true;
    
    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {

if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        
        SQUE_INPUT_Process(0);

        if(have_resumed) SQUE_CHECK_RENDER_ERRORS();

        ImGui_ImplSqueLib_NewFrame();
        if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
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
        SQUE_DISPLAY_GetViewportSize(0, &x, &y);
if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        // Remember to make the expected framebuffer (aka window) to main
        SQUE_DISPLAY_MakeContextMain(0);
        SQUE_RENDER_ChangeFramebufferSize(x, y);
if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        SQUE_RENDER_Clear(col);
        //--
        SQUE_RENDER_UseProgram(program);
        if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        float green = sin(t.ReadMilliSec() / 200.f) + 0.5f;
        SetFloat4(program, "ourColor", glm::vec4(0, green, 0, 1));

        SQUE_SetActiveTextureUnit(SQUE_TEXTURE0);
        SQUE_BindTexture(tex);

        SQUE_RENDER_DrawIndices(triangle);
        if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        
        //--
if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        ImGui_ImplSqueLib_Render(ImGui::GetDrawData());
if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        SQUE_DISPLAY_SwapAllBuffers();
if(have_resumed) SQUE_CHECK_RENDER_ERRORS();
        
        
    }

    //----------------------
    triangle.verts = NULL;
    triangle.indices = NULL;
    //----------------------
    ImGui_ImplSqueLib_Shutdown();
}

int main(int argc, char**argv)
{
    main_states state = MAIN_CREATION;
    bool ret = true;

    // Helpers Initialization
    {
        SQUE_LIB_Init("Squelib Testing Grounds", NULL);
    }

    // Engine Initialization
    {
        SQUE_LOG(SQUE_LogType::LT_INFO, "Initializing Engine...");
        // Initialize all modules in required order

        state = (ret) ? MAIN_UPDATE : MAIN_EXIT;

        if (ret) { SQUE_LOG(SQUE_LogType::LT_INFO, "Entering Update Loop..."); }
        else SQUE_LOG(SQUE_LogType::LT_WARNING, "Error Initializing Engine...");
    }

    //LearnOpenGLTest();
    ImGuiImplSqueLibTest();
    // For Testing timer and android keyboard
    
    while(state == MAIN_UPDATE)
    {
        if(SQUE_DISPLAY_ShouldWindowClose(0))
            state = MAIN_FINISH;
    }

    //  Engine CleanUp
    {
        SQUE_LOG(SQUE_LogType::LT_INFO, "Cleaning Up Engine...");

        // perform the CleanUp of all modules in reverse init order preferably

        if (ret) { SQUE_LOG(SQUE_LogType::LT_INFO, "Application Cleanup Performed Successfully..."); }
        else SQUE_LOG(SQUE_LogType::LT_WARNING, "Application Cleanup with errors...");

        // Delete memory bound modules
    }

    // Helpers CleanUp
    {
        SQUE_LOG(SQUE_LogType::LT_INFO, "Finishing Executing Engine...");
        // Close something that is not part of the engine as a module

        SQUE_LOG(SQUE_LogType::LT_INFO, "Closing Helpers...");
        SQUE_LIB_Close();
    }

    return 0;
}