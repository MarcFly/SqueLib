#include <squelib.h>
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

static ColorRGBA clear_color(0,0,0,1);

static inline void PreprocessFrame()
{
    SQUE_INPUT_Process(0);
    if (on_background)
    {
        SQUE_Sleep(100);
    }
    else
    {
        ImGui_ImplSqueLib_NewFrame();
        ImGui::NewFrame();
    }
}

static inline void PostprocessFrame()
{


    ImGui::Render();

    int32_t x, y;
    SQUE_DISPLAY_GetViewportSize(&x, &y, 0);

    // Remember to make the expected framebuffer (aka window) to main
    SQUE_DISPLAY_MakeContextMain(0);
    SQUE_RENDER_ChangeFramebufferSize(x, y);

    SQUE_RENDER_Clear(clear_color);

    ImGui_ImplSqueLib_Render(ImGui::GetDrawData());
    SQUE_DISPLAY_SwapAllBuffers();
}

int main (int argc, char** argv)
{
    SQUE_LIB_Init("SqueLifeApp");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    ImGui_ImplSqueLib_Init();

    while (!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        PreprocessFrame();
        if (!on_background)
        {
            // Do the App here

            PostprocessFrame();
        }        
    }

    SQUE_LIB_Close();
    return 0;
}