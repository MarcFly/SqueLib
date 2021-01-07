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
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";
#else
const char* vertexShaderSource = "#version 320 es\n"
"precision mediump float;"
"in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";
const char* fragmentShaderSource = "#version 320 es\n"
"precision mediump float;"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
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

    FLY_Mesh mesh;
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };

    GLenum err;

    FLYRENDER_GenBuffer(&mesh, 1);
    mesh.buffers[0]->verts = new float[12]{
         0.5f,  0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
        -0.5f, -0.5f, 0.0f,  // top
        -0.5f,  0.5f, 0.0f
    };
    mesh.buffers[0]->num_verts = 4;
    mesh.buffers[0]->vert_size = 3;

    mesh.buffers[0]->num_index = 6;
    mesh.buffers[0]->indices = new uint32[6]{
        0, 1, 3,
        1, 2, 3        
    };
    SET_FLAG(mesh.buffers[0]->layout, FLYSHADER_LAYOUT_HAS_INDICES);
    FLYRENDER_BufferArray(&mesh);

    FLY_Shader* v_shader = FLYSHADER_Create(FLYSHADER_VERTEX, vertexShaderSource);
    FLY_Shader* f_shader = FLYSHADER_Create(FLYSHADER_FRAGMENT, fragmentShaderSource);
    v_shader->Compile();
    f_shader->Compile();

    FLY_Program* prog = FLYSHADER_CreateProgram(FLYSHADER_LAYOUT_HAS_INDICES);
    prog->AttachShader(&v_shader);
    prog->AttachShader(&f_shader);
    prog->Link();
    prog->EnableAttributes();

    FLY_Uniform* ourColor = new FLY_Uniform();
    ourColor->name = "ourColor";
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
        int v_color_loc = prog->GetUniformLocation("ourColor");
        glUniform4f(v_color_loc, 0.f, green, 0.f, 1.f);
        prog->Draw(mesh.buffers[0]->attribute_object, mesh.buffers[0]->num_index);

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

        FLYLOG(FLY_LogType::LT_INFO, "Closing Helpers...");
        FLYLIB_Close();
    }

    return 0;
}