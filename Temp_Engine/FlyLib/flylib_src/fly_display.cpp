
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
    int32_t width = 0, height = 0;

    // Initialization Flags
    int32_t* window_options = NULL;
    int32_t num_window_options = 0;
    
    int32_t* context_options = NULL;
    int32_t num_context_options = 0;
    
    int32_t* buffer_options = NULL;
    int32_t num_buffer_options = 0;
    // Flags for workign with flylib
    uint16_t working_flags;
};

static std::mutex display_mtx;
std::vector<FLY_Window*> fly_windows;
FLY_Window* next_window = NULL;
static uint16_t main_window_context = UINT16_MAX;
int monitor_count;

void DebugResizeCallback(int32_t width, int32_t height) { FLYPRINT(LT_INFO, "ViewportResize Callback Not Set, tried: %d,%d!", width, height); }
ResizeCallback viewport_resize_callback = DebugResizeCallback;

void DebugViewportSizeCallback(int32_t* width, int32_t* height) { FLYPRINT(LT_INFO, "Get Viewport Size Callback not Set!"); }
ViewportSizeCallback viewport_size_callback = DebugViewportSizeCallback;

static int32_t def_window_options[] =
{
    FLY_DISPLAY_END
};

static int32_t const def_context_options[] =
{
    FLY_WINDOW_CONTEXT_MAJOR, FLY_CONTEXT_MAJOR_MIN,
    FLY_DISPLAY_END
};

static int32_t const def_buffer_options[] =
{
    FLY_RED_BITS, 8,
    FLY_GREEN_BITS, 8,
    FLY_BLUE_BITS, 8,
    FLY_ALPHA_BITS, 8,
    FLY_BUFFER_SIZE, 32,
    FLY_STENCIL_BITS, 0,
    FLY_DEPTH_BITS, 24,
    FLY_RENDERABLE_TYPE, FLY_MIN_RENDERABLE,
    FLY_DISPLAY_END
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLATFORM SPECIFICS ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(ANDROID)
    extern int graphics_backend_started;
    extern struct android_app* my_app;
#   include <android/native_activity.h>
#   include <android_native_app_glue.h>
#   include <android/configuration.h>
#elif defined (_WIN32) || defined (__linux__)

#endif

#if defined(USE_EGL)
#   include <EGL/egl.h>
#   include <GLES3/gl32.h>
    EGLNativeWindowType egl_window;
    EGLDisplay egl_display;   // This is what I call window (egl swaps it <><><><>)
    EGLSurface egl_surface = EGL_NO_SURFACE;
    EGLContext egl_context;


#elif defined(USE_GLFW)
#   include <GLFW/glfw3.h>
#   include <GLFW/glfw3native.h>
    GLFWmonitor** glfw_monitors;
    std::vector<GLFWwindow*> glfw_windows;
    
    void GLFW_ErrorCallback(int error_code, const char* description)
    {
        FLYPRINT(FLY_LogType::LT_ERROR, "GLFW_ERROR-%d: %s", error_code, description);
    }

    void GLFW_CloseCallback(GLFWwindow* window)
    {
        for (int i = 0; i < glfw_windows.size(); ++i)
            if (glfw_windows[i] == window) FLYDISPLAY_SetWindowClose(i);
    }

    int32_t GetFLY_WindowFromGLFWwindow(GLFWwindow* window)
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
        int32_t win = GetFLY_WindowFromGLFWwindow(window);   
        if (win < 0 || win >= fly_windows.size())
        {
            FLYPRINT(LT_WARNING, "Rogue GLFW window!");
            return;
        }
        FLYDISPLAY_ResizeWindow(win, width, height);
        
        FLYPRINT(FLY_LogType::LT_INFO, "Resized Window \"%s\": %d %d", fly_windows[win]->title, width, height);    
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
    // What if some dipshit calls an Init 2 times?
    // SHould I have a callback that is dereferenced?
    // Dereference would throw an error immediately which is good for people to learn
    // But some things and modules require to be reinitialized after coming back...
    // Should I have a bool?
    // think about it.
    if (!next_window)
    {
        FLYPRINT(FLY_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new FLY_Window();
        // Default window hints...
        next_window->buffer_options = (int32_t*)def_buffer_options;
        next_window->context_options = (int32_t*)def_context_options;
        next_window->context_options = (int32_t*)def_window_options;
    }

    FLYPRINT(LT_INFO, "Declaring Backed Specific Variables...");

    // BackEnd Specific Variables
#if defined(USE_EGL)
    EGLint egl_major, egl_minor;
#elif defined USE_GLFW
    int glfw_major, glfw_minor, glfw_revision;
    glfwSetErrorCallback(GLFW_ErrorCallback);
#endif

#if defined(ANDROID)
    FLYPRINT(LT_INFO, "ANDROID - Waiting of Graphics Backend Initialization...");
	int events;
	while( !graphics_backend_started )
	{
		struct android_poll_source* source;
		if (ALooper_pollAll( 0, 0, &events, (void**)&source) >= 0)
		{
			if (source != NULL) source->process(my_app, source);
		}
	}
#endif

    FLYPRINT(LT_INFO, "Initializing Display Backend...");
// Backend Initialization
#if defined(USE_EGL)
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(egl_display == EGL_NO_DISPLAY)
    {
        FLYPRINT(FLY_LogType::LT_ERROR, "EGL Found no Display!");
        exit(0);
    }
    FLYPRINT(FLY_LogType::LT_INFO, "Found main display with EGL!");

    if(!eglInitialize(egl_display, &egl_major, &egl_minor))
    {
        FLYPRINT(FLY_LogType::LT_ERROR, "EGL failed to Initialize!");
        exit(0);
    }
    FLYPRINT(FLY_LogType::LT_INFO, "Successfully Initialized EGL!");

    FLYPRINT(FLY_LogType::LT_INFO, "EGL_VERSION: \"%s\" \nEGL_VENDOR: \"%s\"\nEGL_EXTENSIONS: \"%s\"",
        eglQueryString(egl_display, EGL_VERSION), 
        eglQueryString(egl_display, EGL_VENDOR), 
        eglQueryString(egl_display, EGL_EXTENSIONS));

    EGLConfig config;
    EGLint num_config;

    FLYPRINT(FLY_LogType::LT_INFO, "Preparing Config...");
    eglChooseConfig(egl_display, def_buffer_options, &config, 1, &num_config);
    FLYPRINT(FLY_LogType::LT_INFO, "Using EGL Config %d", num_config);

    FLYPRINT(FLY_LogType::LT_INFO, "Creating EGL Context...");

    egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, def_context_options);
    if (egl_context == EGL_NO_CONTEXT)
    {
        FLYPRINT(FLY_LogType::LT_ERROR, "Could not create EGL Context!");
        exit(0);
    }
    FLYPRINT(FLY_LogType::LT_INFO, "Created EGL Context!");

    if (egl_window)
    {
#if defined(ANDROID)
        if (!my_app->window)
        {
            FLYPRINT(FLY_LogType::LT_WARNING, "App restarted without creating window, stopping...");
            exit(0);
        }
#endif
    }
#if defined(ANDROID)
    FLYPRINT(FLY_LogType::LT_INFO, "Creating EGL window = surface");
    egl_window = my_app->window;
#endif
    if (!egl_window)
    {
        FLYPRINT(FLY_LogType::LT_ERROR, "EGL Window got dereferenced!");
        exit(0);
    }

#if defined(ANDROID)
    int32_t width = ANativeWindow_getWidth(egl_window);
    int32_t height = ANativeWindow_getHeight(egl_window);
    next_window->width = width;
    next_window->height = height;
    EGLSurface test = eglCreateWindowSurface(egl_display, config, egl_window, def_window_options);
    FLYPRINT(FLY_LogType::LT_INFO, "ANDROID - Surface Size: %d %d", width, height);
#else // Other EGL based backends?
#endif
    
    
    if (test == EGL_NO_SURFACE)
    {
        if(egl_surface == EGL_NO_SURFACE)
        {
        FLYPRINT(FLY_LogType::LT_ERROR, "Failed to create EGL Surface!: %#x", eglGetError());
        exit(0);
        }
    }
    else
        egl_surface = test;

    FLYPRINT(FLY_LogType::LT_INFO, "Created EGL Surface!");

#elif defined(USE_GLFW)
    FLYPRINT(FLY_LogType::LT_INFO, "Compiled with GLFW Version: %d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    glfwGetVersion(&glfw_major, &glfw_minor, &glfw_revision);
    FLYPRINT(FLY_LogType::LT_INFO, "Using GLFW Version: %d.%d.%d", glfw_major, glfw_minor, glfw_revision);

// GLFW Hints - Before calling glfwInit(), setup behaviour.
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE); // It is default value but for test and check
    if (!glfwInit()) exit(0);
    FLYPRINT(FLY_LogType::LT_INFO, "Successfully Initialized GLFW!");

    glfw_monitors = glfwGetMonitors(&monitor_count); // Main Monitor is always 0
    //glfwSetMonitorCallback( _send_monitor_change_event_ );
#endif

}

void FLYDISPLAY_Close()
{
    FLYPRINT(FLY_LogType::LT_INFO, "Closing FLY_Displays...");
#if defined(USE_EGL)

#elif defined USE_GLFW
    uint16_t glfw_size = glfw_windows.size();
    for (int i = 0; i < glfw_size; ++i)
        glfwDestroyWindow(glfw_windows[i]);
    glfw_windows.clear();
    glfwTerminate();
#endif
    uint16_t size = fly_windows.size();
    for (int i = 0; i < size; ++i)
        delete fly_windows[i];
    fly_windows.clear();
}

void FLYDISPLAY_SetVSYNC(int16_t vsync_val)
{
    if (main_window_context == UINT16_MAX)
    {
        FLYPRINT(FLY_LogType::LT_WARNING, "No main context to act on, dismissed change of SwapInterval!");
        return;
    }
#if defined(USE_EGL)
    // should pass monitor uint to change refresh rate
    eglSwapInterval(egl_display, vsync_val);
#elif defined USE_GLFW
    glfwSwapInterval(vsync_val);
#endif
}

int32_t FLYDISPLAY_GetDPIDensity(uint16_t window)
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

void FLYDISPLAY_GetMainDisplaySize(uint16_t& w, uint16_t& h)
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
void* FLYDISPLAY_GetPlatformWindowHandle(uint16_t window)
{
#if defined(USE_GLFW)
#   if defined(_WIN32)
    return (void*)glfwGetWin32Window(glfw_windows[window]);
#   endif
#else
#endif

    return nullptr;
}

void FLYDISPLAY_ResizeWindow(uint16_t window, uint16_t w, uint16_t h)
{
    if(window >= fly_windows.size()) return;

    fly_windows[window]->width = w;
    fly_windows[window]->height = h;
#if defined(USE_GLFW)
    glfwSetWindowSize(glfw_windows[window], w, h);
#elif defined(USE_EGL)
#endif
    uint16_t prev_main = main_window_context;
    FLYDISPLAY_MakeContextMain(window);
    viewport_resize_callback(w,h);
    FLYDISPLAY_MakeContextMain(prev_main);
}

void FLYDISPLAY_UpdateNativeWindowSize(uint16_t window)
{
    int32_t w, h;
#if defined(USE_EGL)
    w = ANativeWindow_getWidth(egl_window);
    h = ANativeWindow_getHeight(egl_window);
#else if defined(USE_GLFW)
    glfwGetWindowSize(glfw_windows[window], &w, &h);
#endif

    FLYDISPLAY_ResizeWindow(window, w,h);
}
void FLYDISPLAY_GetWindowSize(uint16_t window, int32_t* x, int32_t* y)
{
    if (window < fly_windows.size())
    {
        *x = fly_windows[window]->width;
        *y = fly_windows[window]->height;
    }
    else
    {
        FLYPRINT(FLY_LogType::LT_WARNING, "Window not available!");
    }
}

void FLYDISPLAY_GetViewportSize(uint16_t window, int32_t* w, int32_t* h)
{
    uint16_t prev_main = main_window_context;
    FLYDISPLAY_MakeContextMain(window);
    if (window < fly_windows.size())
        viewport_size_callback(w, h);
    else
        FLYPRINT(FLY_LogType::LT_WARNING, "Window not available!");

    FLYDISPLAY_MakeContextMain(prev_main);
}


uint16_t FLYDISPLAY_GetAmountWindows() { return fly_windows.size(); }

void FLYDISPLAY_SetWindowClose(uint16_t window)
{
    if (window < fly_windows.size())
        SET_FLAG(fly_windows[window]->working_flags, FLYWINDOW_TO_CLOSE);
    else
        FLYPRINT(LT_WARNING, "Invalid window...");
}

bool FLYDISPLAY_ShouldWindowClose(uint16_t window)
{
    if(window < fly_windows.size())
        return CHK_FLAG(fly_windows[window]->working_flags, FLYWINDOW_TO_CLOSE);
    return false;
}

uint16_t FLYDISPLAY_CloseWindow(uint16_t window)
{
    uint16_t size = fly_windows.size();
    if(window < size)
    {
        FLYPRINT(FLY_LogType::LT_INFO, "Closing window n*%d with title %s", window, fly_windows[window]->title);
        delete fly_windows[window];
        fly_windows[window] = fly_windows[size-1];
        fly_windows.pop_back();

#if defined(USE_EGL)

#elif defined(USE_GLFW)
        glfwDestroyWindow(glfw_windows[window]);
        glfw_windows[window] = glfw_windows[size - 1]; // This changes the order drastically, but fast
        glfw_windows.pop_back();
#endif
        FLYPRINT(FLY_LogType::LT_INFO, "Window n*%d with title %s is now n*%d", size-1, fly_windows[window]->title, window);
    }
    else
    {
        FLYPRINT(FLY_LogType::LT_WARNING, "Window n*%d not available!");
    }
    return window; // When a window is popped that is not last, last becomes this window, thus return position of previous last window
}

void FLYDISPLAY_NextWindow_WindowHints(int32_t* options, int32_t size)
{
    if (!next_window)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new FLY_Window();
    }

    next_window->window_options = options;
    next_window->num_window_options = size;
}

void FLYDISPLAY_NextWindow_ContextHints(int32_t* options, int32_t size)
{
    if (!next_window)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new FLY_Window();
    }

    next_window->context_options = options;
    next_window->num_context_options = size;
}

void FLYDISPLAY_NextWindow_BufferHints(int32_t* options, int32_t size)
{
    if (!next_window)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new FLY_Window();
    }

    next_window->buffer_options = options;
    next_window->num_buffer_options = size;
}


void FLYDISPLAY_OpenWindow(const char* title, int32_t width, int32_t height, uint16_t monitor)
{
#if defined(ANDROID)
    if(fly_windows.size() > 0)
    {
        FLYPRINT(LT_WARNING, "On Android I am not supporting multiple windows currently...");
        return;
    }
#endif

    // Set to monitor[0] for fullscreen at 4th parameter
    if (!next_window)
    {
        FLYPRINT(FLY_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new FLY_Window();
        // Default window hints...
        next_window->buffer_options = (int32_t*)def_buffer_options;
        next_window->context_options = (int32_t*)def_context_options;
        next_window->context_options = (int32_t*)def_window_options;
    }

    next_window->title = title;
    next_window->working_flags = NULL;

    int x = 0, y = 0, w = 0, h = 0;

#if defined(USE_GLFW)
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

    next_window->width = width;
    next_window->height = height;
    fly_windows.push_back(next_window);
    FLYDISPLAY_MakeContextMain(fly_windows.size()-1);
    next_window = NULL;
}

void FLYDISPLAY_GetWindowPos(uint16_t window, int32_t& x, int32_t& y)
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

void FLYDISPLAY_SwapBuffer(uint16_t window)
{
    if(window > fly_windows.size())
    {
        FLYPRINT(LT_WARNING, "Out of range window...");
        return;
    }
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
        return;
#elif defined(USE_GLFW)
        glfwSwapBuffers(glfw_windows[i]);
#endif
    }
}

void FLYDISPLAY_MakeContextMain(uint16_t window)
{
    if (window > fly_windows.size())
    {
        FLYPRINT(FLY_LogType::LT_WARNING, "Invalid context!");
        return;
    }
#if defined(USE_EGL)
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context))
    {
        FLYPRINT(FLY_LogType::LT_ERROR, "Failed to attach Context to Surface (eglMakeCurrent())!");
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