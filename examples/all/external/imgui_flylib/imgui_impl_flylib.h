#ifndef IMGUI_IMPL_FLYLIB
#define IMGUI_IMPL_FLYLIB

#include <imgui.h>

// Use This along the usual ImGui Structure, FlyLib Init After ImGui CreateContex
IMGUI_IMPL_API void ImGui_ImplFlyLib_Init();        // After ImGui CreateContext
IMGUI_IMPL_API void ImGui_ImplFlyLib_Shutdown();    // Before ImGui Shutdown if still using that
IMGUI_IMPL_API void ImGui_ImplFlyLib_NewFrame();    // Before ImGui NewFrame(), It will not process inputs, just read, you have to process them
IMGUI_IMPL_API void ImGui_ImplFlyLib_Render(ImDrawData* draw_data);

#endif