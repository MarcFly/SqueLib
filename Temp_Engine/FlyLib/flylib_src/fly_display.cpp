
#if defined(DISPLAY_SOLO)
#   include "fly_display.h"
#else
#   include "fly_lib.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DEFINITION ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <mutex>

struct FLY_Window
{
    // Main attributes
    const char* title = "";
    int32 width = 0, height = 0;

    // Initialization Flags
    int2* window_flags = NULL;
    int2* context_flags = NULL;
    int2* buffer_flags = NULL;

    // Flags for workign with flylib
    uint16 working_flags;
};

static std::mutex display_mtx;
std::vector<FLY_Window*> fly_windows;
FLY_Window* next_window = NULL;
static uint16 main_window_context = UINT16_MAX;
int monitor_count;

void EmptyResizeCallback(int32 width, int32 height) { FLYPRINT(LT_INFO, "ViewportResize Callback Not Set, tried: %d,%d!", width, height); }
ResizeCallback viewport_resize_callback = EmptyResizeCallback;

void EmptyViewportSizeCallback(int32* width, int32* height) { FLYPRINT(LT_INFO, "Get Viewport Size Callback not Set!"); }
ViewportSizeCallback viewport_size_callback = EmptyViewportSizeCallback;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLATFORM SPECIFICS ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(ANDROID)
    extern int graphics_backed_started;
    extern struct android_app* my_app;
#   include <android/native_activity.h>
#   include <android_native_app_glue.h>
#   #include <android/configuration.h>
#elif defined (_WIN32) || defined (__linux__)

#endif

static int32 const def_window_options[] =
{
    FLY_DISPLAY_END
};

static int32 const def_context_options[] =
{
    FLY_DISPLAY_END
};

static int32 const def_buffer_options[] =
{
    FLY_RED_BITS, 8,
    FLY_GREEN_BITS, 8,
    FLY_BLUE_BITS, 8,
    FLY_ALPHA_BITS, 8,
    FLY_BUFFER_SIZE, 32,
    FLY_STENCIL_BITS, 8,
    FLY_DEPTH_BITS, 24,
    FLY_WINDOW_CONTEXT_MAJOR,  FLY_CONTEXT_MAJOR_MIN,
    FLY_DISPLAY_END
};


#if defined(USE_EGL)
#   include <EGL/egl.h>
    std::vector<EGLNativeWindowType> egl_windows; // This is what I call display
    std::vector<EGLDisplay> egl_displays;   // This is what I call window (egl swaps it <><><><>)
    
    std::vector<EGLSurface> egl_surfaces;
    std::vector<EGLContext> egl_contexts
    // This should be modifiable accordig to a FLY_RENDER_INIT_VARIABLES (forexample)
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

#elif defined(USE_GLFW)
#   include <GLFW/glfw3.h>
    GLFWmonitor** glfw_monitors;
    std::vector<GLFWwindow*> glfw_windows;
    
    void GLFW_ErrorCallback(int error_code, const char* description)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "GLFW_ERROR-%d: %s", error_code, description);
    }

    void GLFW_CloseCallback(GLFWwindow* window)
    {
        for (int i = 0; i < glfw_windows.size(); ++i)
            if (glfw_windows[i] == window) FLYDISPLAY_SetWindowClose(i);
    }

    int32 GetFLY_WindowFromGLFWwindow(GLFWwindow* window)
    {
        FLY_Window* ret = nullptr;
        for(int i = 0; i < fly_windows.size(); ++i)
        {
            if(glfw_windows[i] == window)
                return i;
        }
        return -1;
    }

    void GLFW_FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        int32 win = GetFLY_WindowFromGLFWwindow(window);   
        if (win < 0 || win >= fly_windows.size())
        {
            FLYPRINT(LT_WARNING, "Rogue GLFW window!");
            return;
        }
        FLYDISPLAY_ResizeWindow(win, width, height);
        
        FLYLOG(FLY_LogType::LT_INFO, "Resized Window \"%s\": %d %d", fly_windows[win]->title, width, height);    
    }

    static void GLFW_MouseEnterLeaveCallback(GLFWwindow* window, int entered)
    {
        for (int i = 0; i < glfw_windows.size(); ++i)
            if (glfw_windows[i] == window)
            {
                (entered > 0) ? SET_FLAG(fly_windows[i]->working_flags, FLYWINDOW_MOUSE_IN) : CLR_FLAG(fly_windows[i]->working_flags, FLYWINDOW_MOUSE_IN);
                break;
            }
    }

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZING & STATE MANAGEMENT ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLYDISPLAY_Init()
{
    FLYLOG(LT_INFO, "Declaring Backed Specific Variables...");

    // BackEnd Specific Variables
#if defined(USE_EGL)
    EGLint egl_major, egl_minor;
#elif defined USE_GLFW
    int glfw_major, glfw_minor, glfw_revision;
    glfwSetErrorCallback(GLFW_ErrorCallback);
#endif


    FLYLOG(LT_INFO, "Initializing Display Backend...");
// Backend Initialization
#if defined(USE_EGL)
    egl_displays.push_back(eglGetDisplay(EGL_DEFAULT_DISPLAY));
    if(egl_display == EGL_NO_DISPLAY)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "EGL Found no Display!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Found main display with EGL!");

    if(!eglInitialize(egl_displays[0], &egl_major, &egl_minor))
    {
        FLYLOG(FLY_LogType::LT_ERROR, "EGL failed to Initialize!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Successfully Initialized EGL!");

    FLYLOG(FLY_LogType::LT_INFO, "EGL_VERSION: \"%s\" \nEGL_VENDOR: \"%s\"\nEGL_EXTENSIONS: \"%s\"",
        eglQueryString(egl_display[0], EGL_VERSION), 
        eglQueryString(egl_display[0], EGL_VENDOR), 
        eglQueryString(egl_display[0], EGL_EXTENSIONS));
    


#elif defined(USE_GLFW)
    FLYLOG(FLY_LogType::LT_INFO, "Compiled with GLFW Version: %d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    glfwGetVersion(&glfw_major, &glfw_minor, &glfw_revision);
    FLYLOG(FLY_LogType::LT_INFO, "Using GLFW Version: %d.%d.%d", glfw_major, glfw_minor, glfw_revision);

// GLFW Hints - Before calling glfwInit(), setup behaviour.
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE); // It is default value but for test and check
    if (!glfwInit()) exit(0);
    FLYLOG(FLY_LogType::LT_INFO, "Successfully Initialized GLFW!");

    glfw_monitors = glfwGetMonitors(&monitor_count); // Main Monitor is always 0
    //glfwSetMonitorCallback( _send_monitor_change_event_ );
#endif

}

void FLYDISPLAY_Close()
{
    FLYLOG(FLY_LogType::LT_INFO, "Closing FLY_Displays...");
#if defined(USE_EGL)

#elif defined USE_GLFW
    uint16 glfw_size = glfw_windows.size();
    for (int i = 0; i < glfw_size; ++i)
        glfwDestroyWindow(glfw_windows[i]);
    glfw_windows.clear();
    glfwTerminate();
#endif
    uint16 size = fly_windows.size();
    for (int i = 0; i < size; ++i)
        delete fly_windows[i];
    fly_windows.clear();
}

void FLYDISPLAY_SetVSYNC(int16 vsync_val)
{
    if (main_window_context == UINT16_MAX)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "No main context to act on, dismissed change of SwapInterval!");
        return;
    }
#if defined(USE_EGL)
    eglSwapInterval(egl_display, vsync_val);
#elif defined USE_GLFW
    glfwSwapInterval(vsync_val);
#endif
}

int32 FLYDISPLAY_GetDPIDensity(uint16 window)
{
#if defined(ANDROID)
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, my_app->activity->assetManager);
    int32_t density = AConfiguration_getDensity(config);
    AConfiguration_delete(config);
    return density;
#elif defined USE_GLFW
    const GLFWvidmode* mode = glfwGetVideoMode(glfw_monitors[0]);
    int width_mm, height_mm;
    glfwGetMonitorPhysicalSize(glfw_monitors[0], &width_mm, &height_mm);
    float inches = (width_mm * height_mm) / 25.4f;

    return (mode->width * mode->height) / inches;
#endif

    return 400;

}

void FLYDISPLAY_GetMainDisplaySize(uint16& w, uint16& h)
{
#if defined(USE_GLFW)
    const GLFWvidmode* mode = glfwGetVideoMode(glfw_monitors[0]);
    w = mode->width;
    h = mode->height;
#elif defined(USE_EGL)
    w = fly_windows[0]->width;
    h = fly_windows[0]->height;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROL SPECIFIC WINDOWS //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <GLFW/glfw3native.h>

void* FLYDISPLAY_GetPlatformWindowHandle(uint16 window)
{
#if defined(USE_GLFW)
#   if defined(_WIN32)
    return (void*)glfwGetWin32Window(glfw_windows[window]);
#   endif
#else
#endif

    return nullptr;
}

void FLYDISPLAY_ResizeWindow(uint16 window, uint16 w, uint16 h)
{
    if(window >= fly_windows.size()) return;

    fly_windows[window]->width = w;
    fly_windows[window]->height = h;
#if defined(USE_GLFW)
    glfwSetWindowSize(glfw_windows[window], w, h);
#elif defined(USE_EGL)
    // ?
#endif
    uint16 prev_main = main_window_context;
    FLYDISPLAY_MakeContextMain(window);
    viewport_resize_callback(w,h);
    FLYDISPLAY_MakeContextMain(prev_main);
}

// Unsure if this returns content rect or actual window size on GLFW, because it resizes to content rect I think
void FLYDISPLAY_GetWindowSize(uint16 window, int32* x, int32* y)
{
    if (window < fly_windows.size())
    {
        *x = fly_windows[window]->width;
        *y = fly_windows[window]->height;
    }
    else
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Window not available!");
    }
}

void FLYDISPLAY_GetViewportSize(uint16 window, int32* w, int32* h)
{
    uint16 prev_main = main_window_context;
    FLYDISPLAY_MakeContextMain(window);
    if (window < fly_windows.size())
        viewport_size_callback(w, h);
    else
        FLYLOG(FLY_LogType::LT_WARNING, "Window not available!");

    FLYDISPLAY_MakeContextMain(prev_main);
}


uint16 FLYDISPLAY_GetAmountWindows() { return fly_windows.size(); }

void FLYDISPLAY_SetWindowClose(uint16 window)
{
    if (window < fly_windows.size())
        SET_FLAG(fly_windows[window]->working_flags, FLYWINDOW_TO_CLOSE);
    else
        FLYLOG(LT_WARNING, "Invalid window...");
}

bool FLYDISPLAY_ShouldWindowClose(uint16 window)
{
    if(window < fly_windows.size())
        return CHK_FLAG(fly_windows[window]->working_flags, FLYWINDOW_TO_CLOSE);
    return false;
}

uint16 FLYDISPLAY_CloseWindow(uint16 window)
{
    uint16 size = fly_windows.size();
    if(window < size)
    {
        FLYLOG(FLY_LogType::LT_INFO, "Closing window n*%d with title %s", window, fly_windows[window]->title);
        delete fly_windows[window];
        fly_windows[window] = fly_windows[size-1];
        fly_windows.pop_back();

#if defined(USE_EGL)

#elif defined(USE_GLFW)
        glfwDestroyWindow(glfw_windows[window]);
        glfw_windows[window] = glfw_windows[size - 1]; // This changes the order drastically, but fast
        glfw_windows.pop_back();
#endif
        FLYLOG(FLY_LogType::LT_INFO, "Window n*%d with title %s is now n*%d", size-1, fly_windows[window]->title, window);
    }
    else
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Window n*%d not available!");
    }
    return window; // When a window is popped that is not last, last becomes this window, thus return position of previous last window
}

void FLYDISPLAY_OpenWindow(const char* title, int32 width, int32 height, uint16 monitor)
{
    // Set to monitor[0] for fullscreen at 4th parameter
    if (!next_window)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new FLY_Window();
        // Default window hints...
    }

    next_window->title = title;
    next_window->working_flags = NULL;

    //FLYDISPLAY_NextWindowOptions(window->flags, window->num_flags);

    int x = 0, y = 0, w = 0, h = 0;

#if defined(USE_EGL)
#if defined(ANDROID)
    if (egl_contexts.size() > 0)
    {
        FLYPRINT(LT_WARNING, "On Android I am not supporting multiple windows currently...");
        return;
    }
#endif

    EGLConfig config;
    EGLint num_config;
    FLYLOG(FLY_LogType::LT_INFO, "Preparing Config...");
    eglChooseConfig(egl_display[monitor], next_window->buffer_flags, &config, 1, &num_config);
    FLYLOG(FLY_LogType::LT_INFO, "Using EGL Config %d", num_config);

    FLYLOG(FLY_LogType::LT_INFO, "Creating EGL Context...");

    egl_contexts.push_back(eglCreateContext(egl_display[monitor], config, EGL_NO_CONTEXT, next_window->context_flags));
    if (egl_contexts[egl_contexts.size() - 1] == EGL_NO_CONTEXT)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Could not create EGL Context!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Created EGL Context!");

    egl_windows.push_back(EGLNativeWindowType());
    if (egl_windows[egl_windows.size() - 1])
    {
#if defined(ANDROID)
        if (!my_app->window)
        {
            FLYLOG(FLY_LogType::LT_WARNING, "App restarted without creating window, stopping...");
            exit(0);
        }
        else
        {
            FLYLOG(FLY_LogType::LT_INFO, "Getting EGL Surface..");
            egl_windows[egl_windows.size() - 1] = my_app->window;
        }
#endif
    }

    if (!egl_windows[egl_windows.size()-1])
    {
        FLYLOG(FLY_LogType::LT_ERROR, "EGL Window got dereferenced!");
        exit(0);
    }

#if defined(ANDROID)
    width = ANativeWindow_getWidth(egl_windows[egl_windows.size()-1]);
    height = ANativeWindow_getHeight(egl_windows[egl_windows.size() - 1]);
    egl_surfaces.push_back(eglCreateWindowSurface(egl_displays[monitor], config, my_app->window, next_window->window_options));
    FLYLOG(FLY_LogType::LT_INFO, "ANDROID - Surface Size: %d %d", width, height);
#else // Other EGL based backends?
#endif

    if (egl_surface == EGL_NO_SURFACE)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Failed to create EGL Surface!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Created EGL Surface!");

#elif defined(USE_GLFW)
    glfwGetMonitorWorkarea(glfw_monitors[monitor], &x, &y, &w, &h);
    width = (width != 0) ?width : w * .7;
    height = (height != 0) ? height : h * .7;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!glfw_window)
    {
        FLYPRINT(LT_WARNING, "Unable to create GLFW window...");
        return;
    }
    glfw_windows.push_back(glfw_window);
    glfwSetWindowCloseCallback(glfw_window, GLFW_CloseCallback);
    glfwSetFramebufferSizeCallback(glfw_window, GLFW_FramebufferResizeCallback);
    glfwSetCursorEnterCallback(glfw_window, GLFW_MouseEnterLeaveCallback);
    FLYPRINT(FLY_LogType::LT_INFO, "GLFW Window Created!");
#endif

    FLYPRINT(FLY_LogType::LT_INFO, "Window \"%s\" opened correctly", title);

    FLYDISPLAY_MakeContextMain(fly_windows.size()-1);
    next_window->width = width;
    next_window->height = height;
    fly_windows.push_back(next_window);
    next_window = NULL;
}

void FLYDISPLAY_GetWindowPos(uint16 window, int32& x, int32& y)
{
#if defined(USE_GLFW)
    glfwGetWindowPos(glfw_windows[window], &x, &y);
#else
    //?
    x = 0; 
    y = 0;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROLLING CONTEXTS //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLYDISPLAY_SwapBuffer(uint16 window)
{
#if defined(USE_EGL)
    // Revise when i get a multiple window frontend for egl
    eglSwapBuffers(egl_display, egl_surface);
#elif defined USE_GLFW
    glfwSwapBuffers(glfw_windows[window]);
#endif
}

void FLYDISPLAY_SwapAllBuffers()
{
    for(int i = 0; i < fly_windows.size(); ++i)
    {   
        FLYDISPLAY_MakeContextMain(i);
#if defined(USE_EGL)
        eglSwapBuffers(egl_display, egl_surface);
        fly_windows[i]->width = ANativeWindow_getWidth(egl_window);
        fly_windows[i]->height = ANativeWindow_getHeight(egl_window);
#elif defined(USE_GLFW)
        glfwSwapBuffers(glfw_windows[i]);
#endif
    }
}

void FLYDISPLAY_MakeContextMain(uint16 window)
{
    if (window > fly_windows.size())
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Invalid context!");
        return;
    }
#if defined(USE_EGL)
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context))
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Failed to attach Context to Surface (eglMakeCurrent())!");
        exit(0);
    };
#elif defined(USE_GLFW)
    glfwMakeContextCurrent(glfw_windows[window]);
#endif
    main_window_context = window;
}

ResizeCallback FLYDISPLAY_SetViewportResizeCallback(ResizeCallback viewport_cb)
{
    ResizeCallback ret = viewport_resize_callback;
    viewport_resize_callback = viewport_cb;
    return ret;
}

ViewportSizeCallback FLYDISPLAY_SetViewportSizeCallback(ViewportSizeCallback viewport_size_cb)
{
    ViewportSizeCallback ret = viewport_size_callback;
    viewport_size_callback = viewport_size_cb;
    return ret;
}