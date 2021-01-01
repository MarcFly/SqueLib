#ifdef RENDER_SOLO
#   include "fly_render.h"
#else
#   include "fly_lib.h"
#endif

#include <glad/glad.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl3platform.h>
#ifdef USE_GLFW

#   include <GLFW/glfw3.h>
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

#endif

bool FLYRENDER_Init()
{
    bool ret = true;

#ifdef USE_OPENGL
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        FLYLOG(FLY_LogType::LT_CRITICAL, "Couldn't Initialize GLAD...");
        return false;
    }
#elif defined USE_OPENGLES
    //if(!gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress))
#endif
    

    FLYLOG(FLY_LogType::LT_INFO, "GLAD Initialized!");
#ifdef USE_OPENGL
    FLYLOG(FLY_LogType::LT_INFO, "OpenGL/ES Version: %d.%d", GLVersion.major, GLVersion.minor);
#elif defined USE_OPENGLES
    FLYLOG(FLY_LogType::LT_INFO, "OpenGLES Version: %d.%d", GLVersion.major, GLVersion.minor);
#endif
    // Generate Viewport with window size
    glViewport(0,0,100,100);

    return ret;
}

void FLYRENDER_Clear(int clear_flags, ColorRGBA* color)
{
    if(color != NULL) glClearColor(color->r, color->g, color->b, color->a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

const char* FLYRENDER_GetGLSLVer()
{
    return "#version 150";
}

void ForceLoadGL()
{
    gladLoadGL();
}