
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
#elif defined _WIN32 || defined __linux__
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
        FLYLOG(FLY_LogType::LT_ERROR, "GLFW_ERROR-%d: %s", error_code, description);
    }
    int monitor_count;
    GLFWmonitor** glfw_monitors;
    std::vector<GLFWwindow*> glfw_windows;
#endif

// Platfrom Agnostic Includes and Variables
#include <mutex>
static std::mutex _mtx;
static std::vector<FLY_Window*> fly_windows;
static uint16 main_window_context = UINT16_MAX;

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZING & GLOBAL STATE CONTROL ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FLYDISPLAY_Init(uint16 flags, const char* title, uint16 w, uint16 h)
{
    bool ret = true;

    FLY_Window* init_window = new FLY_Window();
    init_window->title = title;
    init_window->width = w;
    init_window->height = h;
    init_window->flags |= flags;
    fly_windows.push_back(init_window);

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
        FLYLOG(FLY_LogType::LT_ERROR, "EGL Found no Display!");
        return false;
    }
    FLYLOG(FLY_LogType::LT_INFO, "Found main display with EGL!");

    if(!eglInitialize(egl_display, &egl_major, &egl_minor))
    {
        FLYLOG(FLY_LogType::LT_ERROR, "EGL failed to Initialize!");
        return false;
    }
    FLYLOG(FLY_LogType::LT_INFO, "Successfully Initialized EGL!");

    FLYLOG(FLY_LogType::LT_INFO, "EGL_VERSION: \"%s\" \nEGL_VENDOR: \"%s\"\nEGL_EXTENSIONS: \"%s\"",
        eglQueryString(egl_display, EGL_VERSION), 
        eglQueryString(egl_display, EGL_VENDOR), 
        eglQueryString(egl_display, EGL_EXTENSIONS));
    
    eglChooseConfig(egl_display, config_attribute_list, &config, 1, &num_config);
    FLYLOG(FLY_LogType::LT_INFO, "Using EGL Config %d", num_config);

    FLYLOG(FLY_LogType::LT_INFO, "Creating EGL Context...");
    egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attribute_list);
    if(egl_context == EGL_NO_CONTEXT)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Could not create EGL Context!");
        return false;
    }
    FLYLOG(FLY_LogType::LT_INFO, "Created EGL Context!");

#ifdef ANDROID
    if( egl_window && !app->window)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "App restarted without creating window, stopping...");
        exit(0);
    }
#endif

    FLYLOG(FLY_LogType::LT_INFO, "Getting EGL Surface..");
    if(!egl_window)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Could not get EGL_Window!");
        return false;
    }

#ifdef ANDROID
    init_window->width = ANativeWindow_getWidth(egl_window);
    init_window->height= ANativeWindow_getHeight(egl_window);
    egl_surface = eglCreateWindowSurface(egl_display, config, app->window, window_attribute_list);
#endif

    if(egl_surface == EGL_NO_SURFACE)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Failed to create EGL Surface!");
        return false;
    }
    FLYLOG(FLY_LogType::LT_INFO, "Created EGL Surface!");

    if(!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context))
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Failed to attach Context to Surface (eglMakeCurrent())!");
        return false;
    }
#elif defined USE_GLFW
// If SetErrorCallback is correct, no need to do flylogs

    FLYLOG(FLY_LogType::LT_INFO, "Compiled with GLFW Version: %d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    glfwGetVersion(&glfw_major, &glfw_minor, &glfw_revision);
    FLYLOG(FLY_LogType::LT_INFO, "Using GLFW Version: %d.%d.%d", glfw_major, glfw_minor, glfw_revision);

// GLFW Hints - Before calling glfwInit(), setup behaviour.
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE); // It is default value but for test and check
    if(!glfwInit()) return false;
    FLYLOG(FLY_LogType::LT_INFO, "Successfully Initialized GLFW!");

    glfw_monitors = glfwGetMonitors(&monitor_count); // Main Monitor is always 0
    //glfwSetMonitorCallback( _send_monitor_change_event_ );

    if(!FLYDISPLAY_OpenWindow(init_window)) return false;
    FLYLOG(FLY_LogType::LT_INFO, "Opened main GLFW window!");
#endif



    
    return ret;
}


bool FLYDISPLAY_Close()
{
    bool ret = true;
    FLYLOG(FLY_LogType::LT_INFO, "Closing FLY_Displays...");
#ifdef USE_EGL
#elif defined USE_GLFW

    for (int i = 0; i < glfw_windows.size(); ++i)
        glfwDestroyWindow(glfw_windows[i]);
    glfw_windows.clear();
    glfwTerminate();
#endif
    return ret;
}

void FLYDISPLAY_SetVSYNC(int16_t vsync_val)
{
    if (main_window_context == UINT16_MAX)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "No main context to act on, dismissed change of SwapInterval!");
        return;
    }
#ifdef USE_EGL
    eglSwapInterval(egl_display, vsync_val);
#elif defined USE_GLFW
    glfwSwapInterval(vsync_val);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROL SPECIFIC WINDOWS //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLYDISPLAY_Resize(uint16 window, uint16 w, uint16 h)
{

}

void FLYDISPLAY_GetSize(uint16 window, uint16* x, uint16* y)
{
    uint16 size = fly_windows.size();
    if (size > window)
    {
        *x = fly_windows[window]->width;
        *y = fly_windows[window]->height;
    }
    else
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Window not available!");
    }
}

void FLYDISPLAY_GetAmountWindows(uint16* windows)
{
    *windows = fly_windows.size();
}

bool FLYDISPLAY_ShouldWindowClose(uint16 window)
{
#ifdef USE_EGL
#elif defined USE_GLFW
     //glfwPollEvents();
    return glfwWindowShouldClose(glfw_windows[window]);
#endif
    return false;
}

void FLYDISPLAY_CloseWindow(uint16 window)
{
    uint16 size = fly_windows.size();
    if(window < size)
    {
        FLYLOG(FLY_LogType::LT_INFO, "Closing window n*%d with title %s", window, fly_windows[window]->title);
        delete fly_windows[window];
        fly_windows[window] = fly_windows[size-1];
        fly_windows.pop_back();

#ifdef USE_EGL

#elif defined USE_GLFW
        glfwSetWindowShouldClose(glfw_windows[window], GLFW_TRUE);
        glfw_windows[window] = glfw_windows[size - 1];
        glfw_windows.pop_back();
#endif
        FLYLOG(FLY_LogType::LT_INFO, "Window n*%d with title %s is now n*%d", size-1, fly_windows[window]->title, window);
    }

    else
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Window n*%d not available!");
    }
}

void FLYDISPLAY_DestroyWindow(uint16 window)
{
    uint16 size = fly_windows.size();
    if(window < size)
    {
        FLYLOG(FLY_LogType::LT_INFO, "Destroying window n*%d with title %s", window, fly_windows[window]->title);
        delete fly_windows[window];
        fly_windows[window] = fly_windows[size-1];
        fly_windows.pop_back();

#ifdef USE_EGL

#elif defined USE_GLFW
        glfwDestroyWindow(glfw_windows[window]);
        glfw_windows[window] = glfw_windows[glfw_windows.size() -1];
        glfw_windows.pop_back();
#endif
        FLYLOG(FLY_LogType::LT_INFO, "Window Destroyed!");
    }
    else
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Window n*%d not available!");
    }
    
}

void FLYDISPLAY_NextWindowOptions(uint16 flags)
{
#ifdef USE_EGL
#elif defined USE_GLFW
    (flags & FLYWINDOW_NOT_RESIZABLE) ? glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE) : glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    (flags & FLYWINDOW_NOT_DECORATED) ? glfwWindowHint(GLFW_DECORATED, GLFW_FALSE) : glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    (flags & FLYWINDOW_ALWAYS_TOP) ? glfwWindowHint(GLFW_FLOATING, GLFW_TRUE) : glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
    (flags & FLYWINDOW_MAXIMIZED) ? glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE) : glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);


    // Private Options
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, );
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, );
    //glfwWindowHint(GLFW_CONTEXT_VERSION_REVISION, );
#endif
}

bool FLYDISPLAY_OpenWindow(FLY_Window* window, uint16 monitor)
{
    // Set to monitor[0] for fullscreen at 4th parameter
    if (!window)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "No window parameters set, creating with defaults...");
        window = new FLY_Window();
        window->title = "";
        window->flags = NULL;
    }

    int x=0,y=0,w=0,h=0;
#ifdef USE_EGL
    w = window->width;
    h = window->height;
#elif defined USE_GLFW
    glfwGetMonitorWorkarea(glfw_monitors[monitor], &x, &y, &w, &h);
    w = (window->width != 0) ? window->width : 7 * w / 10;
    h = (window->height != 0) ? window->height : 7 * h / 10;
#endif
    FLYDISPLAY_NextWindowOptions(window->flags);
#ifdef USE_EGL
#elif defined USE_GLFW
    GLFWwindow* first_window = glfwCreateWindow(w, h, window->title, NULL, NULL);
    if(!first_window) return false;
    glfw_windows.push_back(first_window);
#endif
    FLYLOG(FLY_LogType::LT_INFO, "Window \"%s\" opened correctly", window->title);
    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROLLING CONTEXTS //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLYDISPLAY_Clean()
{

}

void FLYDISPLAY_SwapAllBuffers()
{
    for(int i = 0; i < fly_windows.size(); ++i)
    {
#ifdef USE_EGL

#elif defined USE_GLFW
        glfwSwapBuffers(glfw_windows[i]);
#endif
    }
}

void FLYDISPLAY_SwapBuffers(uint16 window)
{
#ifdef USE_EGL
#elif defined USE_GLFW
    glfwSwapBuffers(glfw_windows[window]);
#endif
}

void FLYDISPLAY_MakeContextMain(uint16 window)
{
    if (window > fly_windows.size())
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Invalid context!");
        return;
    }
#ifdef USE_EGL
#elif defined USE_GLFW
    glfwMakeContextCurrent(glfw_windows[window]);
#endif
}