#include <iostream>
#include <fly_lib.h>
#include <imgui.h>
#ifdef USE_GLFW
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#elif defined USE_EGL
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <imgui_impl_android.h>
#endif

#ifndef ANDROID
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 v_pos;\n"
"layout (location = 1) in vec3 v_color;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(v_pos.x, v_pos.y, v_pos.z, 1.0);\n"
"   ourColor = v_color;\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform vec4 unfColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0)+unfColor;\n"
"}\n\0";
#else
const char* vertexShaderSource = "#version 320 es\n"
"precision mediump float;"
"in vec3 v_pos;\n"
"in vec3 v_normal;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(v_pos.x, v_pos.y, v_pos.z, 1.0);\n"
"   ourColor = v_normal;\n"
"}\0";
const char* fragmentShaderSource = "#version 320 es\n"
"precision mediump float;"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"uniform vec4 unfColor;"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0)+unfColor;\n"
"}\n\0";
#endif

#include <glad/glad.h>
enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

#include <cmath>

int main()
{
    main_states state = MAIN_CREATION;
    bool ret = true;
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Helpers Initialization
    {
        ret = FLYLIB_Init(/*flags*/);
    }
    

#ifdef USE_GLFW
    ImGui_ImplGlfw_InitForOpenGL(FLYDISPLAY_RetrieveMainGLFWwindow(), true);
    FLYLOG(FLY_LogType::LT_INFO, "GLAD init for ImGui...");
    gladLoadGL();
    const char* glsl = "#version 330";
    FLYLOG(FLY_LogType::LT_INFO, "OpenGL3 Init for ImGui...");
    ImGui_ImplOpenGL3_Init(FLYRENDER_GetGLSLVer());
#elif defined USE_EGL
    //gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress);
#endif
    
    ImGui::StyleColorsDark();
    
    FLYLOG(FLY_LogType::LT_INFO, "Android Test ImGui Init...");
    #ifdef ANDROID
    ImGui_ImplAndroidGLES2_Init();
    #endif
    // Engine Initialization
    {
        FLYLOG(FLY_LogType::LT_INFO, "Initializing Engine...");
        // Initialize all modules in required order

        state = (ret) ? MAIN_UPDATE : MAIN_EXIT;

        if (ret) { FLYLOG(FLY_LogType::LT_INFO, "Entering Update Loop..."); }
        else FLYLOG(FLY_LogType::LT_WARNING, "Error Initializing Engine...");
    }


// Testing Info for Render Pipeline Testing
    //TestHardCode();


    float* verts = new float[18]{
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
    };
    FLY_Mesh mesh;
    GLenum err;
    mesh.PrepareBuffers(1);
    mesh.buffers[0]->verts = (char*)verts;
    mesh.buffers[0]->num_verts = 3;
    mesh.buffers[0]->SetVarTypes(FLY_FLOAT, FLY_FLOAT, NULL, NULL);
    mesh.buffers[0]->SetComponentSize(3, 3, 0, 0);
    mesh.buffers[0]->SetToNormalize(false, false,false,false);

    //mesh.buffers[0]->num_index = 6;
    /*mesh.buffers[0]->indices = new uint32[6]{
        0, 1, 3,
        1, 2, 3        
    };*/
    //SET_FLAG(mesh.buffers[0]->layout, FLYSHADER_LAYOUT_HAS_INDICES);
    mesh.SendToGPU();

    FLY_Shader* v_shader = FLYSHADER_Create(FLY_VERTEX_SHADER, vertexShaderSource);
    FLY_Shader* f_shader = FLYSHADER_Create(FLY_FRAGMENT_SHADER, fragmentShaderSource);
    v_shader->Compile();
    f_shader->Compile();

    FLY_Program* prog = FLYSHADER_CreateProgram(NULL);
    prog->AttachShader(&v_shader);
    prog->AttachShader(&f_shader);
    prog->Link();
    prog->EnableAttributes(mesh.buffers[0]);

    FLY_Uniform* ourColor = new FLY_Uniform();
    ourColor->name = "unfColor";
    prog->uniform.push_back(ourColor);
    prog->SetupUniformLocations();

    // Setup the uniforms

    // Update Loop
    FLY_Timer t;
    t.Start();
    FLYINPUT_DisplaySoftwareKeyboard(true);
    while(state == MAIN_UPDATE)
    {
        ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
        FLYRENDER_Clear(NULL, &col);        
        
        prog->Prepare();
        float time_val = t.ReadMilliSec()/1000.;
        float green = sin(time_val) + .5f;
        prog->SetFloat4(ourColor->name, float4(0,green,0,1));
        prog->Draw(mesh.buffers[0]);

        uint16 w, h;
        FLYDISPLAY_GetSize(0, &w, &h);

        #ifdef USE_GLFW
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        #elif defined USE_EGL
        ImGui_ImplAndroidGLES2_NewFrame(w ,h, t.ReadMilliSec());
        #endif        
        
        ImGui::NewFrame();
        
        float posx = (t.ReadMilliSec() / 100.f) -  ((int)((t.ReadMilliSec() / 100.f)/w))*w;
        float posy = (t.ReadMilliSec() / 100.f) -  ((int)((t.ReadMilliSec() / 100.f)/h))*h;
        ImGui::SetNextWindowPos(ImVec2(posx, posy));
        ImGui::Begin("Another Window", &ret);
		ImGui::Text("Hello");
		ImGui::End();
        //FLYPRINT(LT_INFO, "WINDOW AT: %.2f, %.2f", posx, posy);
        if(ImGui::IsMouseDown(0))
            FLYPRINT(LT_INFO, "Reading ImGui Input:\nMouse Button 0: %d\nMouse POS: %.2f,%.2f",ImGui::IsMouseDown(0), ImGui::GetMousePos().x, ImGui::GetMousePos().y);
        
        if(ret)ImGui::ShowDemoWindow(&ret);
        // Update all modules in specific order
        
        // Check for main window for closure, if main window is set to close, close everything
        

        ImGui::Render();

#ifdef USE_OPENGL
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        FLYDISPLAY_SwapAllBuffers();
        

        FLYINPUT_Process(0);
        if (FLYDISPLAY_ShouldWindowClose(0))
        {
            FLYLOG(FLY_LogType::LT_INFO, "Checked Window to Close");
            state = MAIN_FINISH;
        }
        
        
    }

    {
        delete verts;
        delete prog;
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

        FLYLOG(LT_INFO, "ImGui::Shutdown()");
#ifdef USE_EGL
        ImGui_ImplAndroidGLES2_Shutdown();
#elif defined(USE_GLFW)
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
#endif
        //ImGui::Shutdown(imgui_ctx);

        FLYLOG(FLY_LogType::LT_INFO, "Closing Helpers...");
        FLYLIB_Close();
    }

    return 0;
}