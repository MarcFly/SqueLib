#ifndef IMGUI_IMPL_ANDROID_GL2
#define IMGUI_IMPL_ANDROID_GL2

#include <imgui.h>

IMGUI_IMPL_API bool        ImGui_ImplAndroidGLES2_Init();
IMGUI_IMPL_API void        ImGui_ImplAndroidGLES2_Shutdown();
IMGUI_IMPL_API void        ImGui_ImplAndroidGLES2_NewFrame(int width, int height, unsigned int millis);
IMGUI_IMPL_API bool        ImGui_ImplAndroidGLES2_ProcessEvent();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_IMPL_API void        ImGui_ImplAndroidGLES2_InvalidateDeviceObjects();
IMGUI_IMPL_API bool        ImGui_ImplAndroidGLES2_CreateDeviceObjects();

IMGUI_IMPL_API void        ImGui_ImplFlyLib_UpdateMousePosAndButtons();

#endif // IMGUI_IMPL_ANDROID_GL2