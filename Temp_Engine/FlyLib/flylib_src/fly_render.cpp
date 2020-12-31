#ifdef RENDER_SOLO
#   include "fly_render.h"
#else
#   include "fly_lib.h"
#endif

#include <glad/glad.h>

#ifdef USE_IMGUI
#include <imgui_impl_opengl3.h>
#endif

#ifdef ANDROID
#   define USE_EGL
#elif defined _WIN32 || defined __linux__
#   define USE_GLFW
#   include <GLFW/glfw3.h>
#endif

#ifdef USE_GLFW

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif

bool FLYRENDER_Init()
{
    bool ret = true;

#ifdef USE_GLFW
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        FLYLOG(FLY_LogType::LT_CRITICAL, "Couldn't Initialize GLAD...");
        return false;
    }
#endif
    FLYLOG(FLY_LogType::LT_INFO, "GLAD Initialized!");
    FLYLOG(FLY_LogType::LT_INFO, "OpenGL Version: %d.%d", GLVersion.major, GLVersion.minor);
    // Generate Viewport with window size
    glViewport(0,0,100,100);

#ifdef USE_IMGUI
    const char* str = "#version 450";
    ImGui_ImplOpenGL3_Init(str);
#endif
    return ret;
}

void FLYRENDER_Clear(int clear_flags, ColorRGBA* color)
{
    if(color != NULL) glClearColor(color->r, color->g, color->b, color->a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef USE_IMGUI
    ImGui_ImplOpenGL3_NewFrame();
#endif
}