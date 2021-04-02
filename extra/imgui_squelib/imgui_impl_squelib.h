#ifndef IMGUI_IMPL_SQUELIB
#define IMGUI_IMPL_SQUELIB

#include <imgui.h>

// Use This along the usual ImGui Structure, SqueLib Init After ImGui CreateContex
IMGUI_IMPL_API void ImGui_ImplSqueLib_Init();        // After ImGui CreateContext
IMGUI_IMPL_API void ImGui_ImplSqueLib_Shutdown();    // Before ImGui Shutdown if still using that
IMGUI_IMPL_API void ImGui_ImplSqueLib_NewFrame();    // Before ImGui NewFrame(), It will not process inputs, just read, you have to process them
IMGUI_IMPL_API void ImGui_ImplSqueLib_Render(ImDrawData* draw_data);

#endif