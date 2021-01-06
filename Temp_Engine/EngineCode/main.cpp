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

#include <glad/glad.h>
enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

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
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

    FLY_Mesh mesh;
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
    };

    FLYRENDER_GenBuffer(&mesh, 1);
    mesh.buffers[0]->verts = vertices;
    mesh.buffers[0]->num_verts = 3;
    mesh.buffers[0]->vert_size = 3;
    mesh.buffers[0]->buffer_structure |= FLYBUFFER_HAS_VERTEX;
    FLYRENDER_BufferArray(&mesh);
    //FLYRENDER_SetAttributesForBuffer(mesh.buffers[0]);

    FLY_Shader v_shader;
    v_shader.source = vertexShaderSource;
    v_shader.type = FLYSHADER_VERTEX;
    FLY_Shader f_shader;
    f_shader.source = fragmentShaderSource;
    f_shader.type = FLYSHADER_FRAGMENT;
    FLYRENDER_CreateShader(&v_shader);
    FLYRENDER_CreateShader(&f_shader);
    FLYRENDER_CompileShader(&v_shader);
    FLYRENDER_CompileShader(&f_shader);
    
    FLY_Program prog;
    FLYRENDER_CreateShaderProgram(&prog);
    FLY_Shader* shaders[] = {&v_shader, &f_shader};
    FLYRENDER_AttachMultipleShadersToProgram(2, shaders, &prog);
    FLYRENDER_LinkShaderProgram(&prog);
    // Update Loop
    FLY_Timer t;
    t.Start();
    FLYINPUT_DisplaySoftwareKeyboard(true);
    while(state == MAIN_UPDATE)
    {
        FLYINPUT_Process(0);    
        ColorRGBA col = ColorRGBA(.1,.3,.1,1.);
        FLYRENDER_Clear(NULL, &col); 

        glUseProgram(prog.id);
        glBindVertexArray(mesh.buffers[0]->attribute_object);
        glDrawArrays(GL_TRIANGLES, 0, 3);

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
        if(FLYDISPLAY_ShouldWindowClose(0))
        {
            FLYLOG(FLY_LogType::LT_INFO, "Checked Window to Close");
            state = MAIN_FINISH;
        }

        ImGui::Render();

#ifdef USE_OPENGL
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
        FLYDISPLAY_SwapAllBuffers();
         
        
        
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