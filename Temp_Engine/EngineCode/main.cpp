#include <iostream>
#include <fly_lib.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
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
    

    ImGui_ImplGlfw_InitForOpenGL(FLYDISPLAY_RetrieveMainGLFWwindow(), true);
    const char* glsl = "#version 150";
    gladLoadGL();
    ImGui_ImplOpenGL3_Init(FLYRENDER_GetGLSLVer());
    ImGui::StyleColorsDark();
    
    // Engine Initialization
    {
        FLYLOG(FLY_LogType::LT_INFO, "Initializing Engine...");
        // Initialize all modules in required order

        state = (ret) ? MAIN_UPDATE : MAIN_EXIT;

        if (ret) { FLYLOG(FLY_LogType::LT_INFO, "Entering Update Loop..."); }
        else FLYLOG(FLY_LogType::LT_WARNING, "Error Initializing Engine...");
    }

    // Update Loop
    
    while(state == MAIN_UPDATE)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();        
        ImGui::NewFrame();
        if(ret)ImGui::ShowDemoWindow(&ret);
        // Update all modules in specific order
        
        // Check for main window for closure, if main window is set to close, close everything
        if(FLYDISPLAY_ShouldWindowClose(0))
        {
            FLYLOG(FLY_LogType::LT_INFO, "Checked Window to Close");
            state = MAIN_FINISH;
        }
        ColorRGBA col = ColorRGBA(.1,.3,.1,1.);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        FLYDISPLAY_SwapAllBuffers();
        FLYRENDER_Clear(NULL, &col);
        FLYINPUT_Process(0);      
        
        
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