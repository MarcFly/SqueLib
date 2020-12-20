#ifdef INPUT_SOLO
#   include "fly_input.h"
#else
#   include "fly_lib.h"
#endif

typedef struct FLY_Key
{
    int key;
    float state;
    void (*key_callback)();
};

FLY_Key keyboard[NUM_KEYS];

#ifdef ANDROID

#include <android_native_app_glue.h>

int OGLESStarted;

void handle_android_cmd(struct android_app* app, int32_t cmd)
{
    switch(cmd)
    {
        case APP_CMD_INIT_WINDOW:
            if(!OGLESStarted) OGLESStarted = 1;
    } 
}

int32_t handle_android_input(struct android_app* app, AInputEvent* ev)
{

    return 0;
}

#elif defined _WIN32 || defined __linux__
#   define USE_GLFW
#   include <GLFW/glfw3.h>
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {

    }

#endif

void FLYINPUT_Process(uint16 window)
{
#ifdef ANDROID
#elif defined USE_GLFW
    glfwPollEvents();
#endif
}
