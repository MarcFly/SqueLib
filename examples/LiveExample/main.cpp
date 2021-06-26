#include <squelib.h>
#include <imgui.h>
#include <imgui_impl_squelib.h>

int main(int argc, char** argv)
{
    SQUE_LIB_Init("LiveExample");
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplSqueLib_Init();
    
    // Loop
    static ColorRGBA clear_color(.2,.1,.5,1);
    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        SQUE_INPUT_Process(0);
        SQUE_RENDER_Clear(clear_color);
        ImGui_ImplSqueLib_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow(NULL);

        ImGui::Render();
        ImGui_ImplSqueLib_Render(ImGui::GetDrawData());

        SQUE_DISPLAY_SwapAllBuffers();
    }
    // Class all
    SQUE_LIB_Close();

    return 0;
}