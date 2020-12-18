
#ifdef DISPLAY_SOLO
#   include "fly_display.h"
#else
#   include "fly_lib.h"
#endif

// Define which API Backend to USE
#ifdef ANDROID
#   define USE_EGL
    extern int OGLESStarted;
#   include <android/native_activity.h>
#   include <android_native_app_glue.h>
    struct android_app* app;
#elif defined _WIN32
#   define USE_GLFW
#endif

// Include the Necessary Backend for the chosen API
// Default Status for the initialization
#ifdef USE_EGL
#   include <EGL/egl.h>
    EGLNativeWindowType egl_window;
    EGLDisplay egl_display;
    EGLSurface egl_surface;
    static EGLint const config_attribute_list[] = {
        EGL_RED_SIZE, 8, 
        EGL_GREEN_SIZE, 8, 
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_BUFFER_SIZE, 32,
        EGL_STENCIL_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_NONE
    };

    static EGLint window_attribute_list[] = {
        EGL_NONE
    };

    static const EGLint context_attribute_list[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    typedef void FLY_window;
#elif defined USE_GLFW
#   include <GLFW/glfw3.h>
#   include <vector>
    void GLFW_ErrorCallback(int error_code, const char* description)
    {
        FLYLOG(FlyLogType::LT_ERROR, "GLFW_ERROR-%d: %s", error_code, description);
    }
    int monitor_count;
    GLFWmonitor** glfw_monitors;
    std::vector<GLFWwindow*> glfw_windows;
#endif

// Platfrom Agnostic Includes and Variables
#include <mutex>
static std::mutex _mtx;
static int last_size_x=0, last_size_y=0;
static int width, height;

// I want to call it a display better that window
// Display will show whatever you throw it, a window 
// does not semantically hold up

// Functionality:
// Initialize where you display
// Pass flags for expected options
// Get the display as pointer for usage
// thread-safe initialization
// Separate from renderer
// Accepts a texture/framebuffer and can display directly

bool FLYDISPLAY_Init(const char* title, int w, int h /*, flags*/)
{
    bool ret = true;

// PreInitialization of the Window/Context Backend
#ifdef USE_EGL
    EGLint egl_major, egl_minor;
    EGLConfig config;
    EGLint num_config;
    EGLContext egl_context;
#elif defined USE_GLFW
    int glfw_major, glfw_minor, glfw_revision;
    glfwSetErrorCallback(GLFW_ErrorCallback);
#endif

// Platform Specifics PreInitialization
#ifdef ANDROID
    int events;
    while(!OGLESStarted)
    {  
        struct android_poll_source* source;
        if(ALooper_pollAll(0,0,&events, (void**)&source) >= 0)
            if(source != NULL) source->process(app, source);
    }
#endif

// Backend Initialization
#ifdef USE_EGL
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(egl_display == EGL_NO_DISPLAY)
    {
        FLYLOG(FlyLogType::LT_ERROR, "EGL Found no Display!");
        return false;
    }
    FLYLOG(FlyLogType::LT_INFO, "Found main display with EGL!");

    if(!eglInitialize(egl_display, &egl_major, &egl_minor))
    {
        FLYLOG(FlyLogType::LT_ERROR, "EGL failed to Initialize!");
        return false;
    }
    FLYLOG(FlyLogType::LT_INFO, "Successfully Initialized EGL!");

    FLYLOG(FlyLogType::LT_INFO, "EGL_VERSION: \"%s\" \nEGL_VENDOR: \"%s\"\nEGL_EXTENSIONS: \"%s\"",
        eglQueryString(egl_display, EGL_VERSION), 
        eglQueryString(egl_display, EGL_VENDOR), 
        eglQueryString(egl_display, EGL_EXTENSIONS));
    
    eglChooseConfig(egl_display, config_attribute_list, &config, 1, &num_config);
    FLYLOG(FlyLogType::LT_INFO, "Using EGL Config %d", num_config);

    FLYLOG(FlyLogType::LT_INFO, "Creating EGL Context...");
    egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attribute_list);
    if(egl_context == EGL_NO_CONTEXT)
    {
        FLYLOG(FlyLogType::LT_ERROR, "Could not create EGL Context!");
        return false;
    }
    FLYLOG(FlyLogType::LT_INFO, "Created EGL Context!");

#ifdef ANDROID
    if( egl_window && !app->window)
    {
        FLYLOG(FlyLogType::LT_WARNING, "App restarted without creating window, stopping...");
        exit(0);
    }
#endif

    FLYLOG(FlyLogType::LT_INFO, "Getting EGL Surface..");
    if(!egl_window)
    {
        FLYLOG(FlyLogType::LT_ERROR, "Could not get EGL_Window!");
        return false;
    }

#ifdef ANDROID
    width = ANativeWindow_getWidth(egl_window);
    height= ANativeWindow_getHeight(egl_window);
    egl_surface = eglCreateWindowSurface(egl_display, config, app->window, window_attribute_list);
#endif

    if(egl_surface == EGL_NO_SURFACE)
    {
        FLYLOG(FlyLogType::LT_ERROR, "Failed to create EGL Surface!");
        return false;
    }
    FLYLOG(FlyLogType::LT_INFO, "Created EGL Surface!");

    if(!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context))
    {
        FLYLOG(FlyLogType::LT_ERROR, "Failed to attach Context to Surface (eglMakeCurrent())!");
        return false;
    }
#elif defined USE_GLFW
// If SetErrorCallback is correct, no need to do flylogs

// GLFW Hints - Before calling glfwInit(), setup behaviour.
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE); // It is default value but for test and check
    if(!glfwInit()) return false;
    FLYLOG(FlyLogType::LT_INFO, "Successfully Initialized GLFW!");

    glfw_monitors = glfwGetMonitors(&monitor_count); // Main Monitor is always 0
    //glfwSetMonitorCallback( _send_monitor_change_event_ );

    if(!FLYDISPLAY_OpenWindow(width, height, title)) return false;
    FLYLOG(FlyLogType::LT_INFO, "Opened main GLFW window!");
#endif



    
    return ret;
}


bool FLYDISPLAY_Close()
{
    bool ret = true;
#ifdef USE_EGL
#elif defined USE_GLFW

    for (int i = 0; i < glfw_windows.size(); ++i)
        glfwDestroyWindow(glfw_windows[i]);
    glfw_windows.clear();
    glfwTerminate();
#endif
    return ret;
}

void FLYDISPLAY_SetVSYNC(int vsync_val)
{
#ifdef USE_EGL
    eglSwapInterval(egl_display, vsync_val);
#endif
}

void FLYDISPLAY_Clean()
{

}

void FLYDISPLAY_SwapBuffers()
{

}

void FLYDISPLAY_Resize(int w, int h)
{

}

void FLYDISPLAY_GetSize(int* x, int* y)
{
    *x = width;
    *y = height;

    if(*x != last_size_x || *y != last_size_y) FLYDISPLAY_Resize(*x, *y);
}

void FLYDISPLAY_CloseWindow(int window)
{
#ifdef USE_EGL
#elif defined USE_GLFW
    int size = glfw_windows.size();
    if (window < size)
    {
        FLYLOG(FlyLogType::LT_INFO, "Closing window n*%d", window);
        glfwSetWindowShouldClose(glfw_windows[window], GLFW_TRUE);
        glfw_windows[window] = glfw_windows[size - 1];
        glfw_windows.pop_back();
    }
    else
    {
        FLYLOG(FlyLogType::LT_WARNING, "Window n*%d not available!");
    }
#endif
}

void FLYDISPLAY_DestroyWindow(int window)
{
#ifdef USE_EGL
#elif defined USE_GLFW
    glfwDestroyWindow(glfw_windows[window]);
    glfw_windows[window] = glfw_windows[glfw_windows.size() -1];
    glfw_windows.pop_back();

#endif
}

bool FLYDISPLAY_OpenWindow(int width, int height, const char* title, int monitor)
{
    // Set to monitor[0] for fullscreen at 4th parameter
    int x=0,y=0,w=0,h=0;
#ifdef USE_EGL
    w = width;
    h = height;
#elif defined USE_GLFW
    glfwGetMonitorWorkarea(glfw_monitors[monitor], &x, &y, &w, &h);
    w = (width != 0) ? width : 7 * w / 10;
    h = (height != 0) ? height : 7 * h / 10;
#endif
    
#ifdef USE_EGL
#elif defined USE_GLFW
    GLFWwindow* first_window = glfwCreateWindow(w, h, title, NULL, NULL);
    if(!first_window) return false;
    glfw_windows.push_back(first_window);
#endif
    return true;
}