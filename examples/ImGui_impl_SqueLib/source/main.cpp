#include <iostream>
#define NOT_GDB
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

bool on_background = false;
void OnResume()
{
    on_background = false;
}
void OnGoBackground()
{
    on_background = true;
}


void ImGuiImplSqueLibTest()
{
    

    ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
    SQUE_Timer t;
    //-------------------------------------------------------------------
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplSqueLib_Init();

    bool show_demo_window = true;
    bool show_another_window = true;
    
    // Typical Loop, mostly the same as expected in GLFW
    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        SQUE_INPUT_Process(0);
        
        if(on_background)
        {
            SQUE_Sleep(100);
            continue;
        }
        {
            // NewFrame for SqueLib has to go before ImGui because it sets up time steps and some other things
            ImGui_ImplSqueLib_NewFrame();
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
            SQUE_DISPLAY_GetViewportSize(&x, &y, 0);

            // Remember to make the expected framebuffer (aka window) to main
            SQUE_DISPLAY_MakeContextMain(0);
            SQUE_RENDER_ChangeFramebufferSize(x, y);

            SQUE_RENDER_Clear(col);

            ImGui_ImplSqueLib_Render(ImGui::GetDrawData());

            SQUE_DISPLAY_SwapAllBuffers();  
        }      
        
    }

    ImGui_ImplSqueLib_Shutdown();
}

int main(int argc, char**argv)
{
    // Initialization with a Window name and Initialization Flags
    SQUE_LIB_Init("Squelib Testing Grounds", SQ_INIT_DEFAULTS | SQ_INIT_MAX_RENDER_VER);

    SQUE_AddOnGoBackgroundCallback(OnGoBackground);
    SQUE_AddOnResumeCallback(OnResume);
    // There should be some default functions which there are non right now.
    // These functions are primarily Android Oriented but help to unload things and reload them
    // When Going on background in Android, GPU will unload everything, so you have to load them again
    // Non-permanent memory will be destroyed but allocated pointers will be held!

    // Own Application Initialization
    {
        SQUE_LOG(SQUE_LogType::LT_INFO, "Initializing Engine...");
        
        // Do your Initialization
    }

    //LearnOpenGLTest();

    ImGuiImplSqueLibTest();

    //  CleanUp things
    {
        SQUE_LOG(SQUE_LogType::LT_INFO, "Cleaning Up Engine...");

        // Deal with memory allocations and such
    }

    // Close Application
    {
        SQUE_LOG(SQUE_LogType::LT_INFO, "Finishing Executing Engine...");

        // Close your things

        SQUE_LOG(SQUE_LogType::LT_INFO, "Closing Helpers...");
        
        SQUE_LIB_Close();
    }

    return 0;
}