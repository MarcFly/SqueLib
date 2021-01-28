
#if defined(DISPLAY_SOLO)
#   include "sque_display.h"
#else
#   include "squelib.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DEFINITION ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <vector>
#include <mutex>

struct SQUE_Window
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
    // Flags for workign with squelib
    uint16_t working_flags;
};

static std::mutex display_mtx;
std::vector<SQUE_Window*> sque_windows;
SQUE_Window* next_window = NULL;
static uint16_t main_window_context = UINT16_MAX;
int monitor_count;

void DebugResizeCallback(int32_t width, int32_t height) { SQUE_PRINT(LT_INFO, "ViewportResize Callback Not Set, tried: %d,%d!", width, height); }
ResizeCallback viewport_resize_callback = DebugResizeCallback;

void DebugViewportSizeCallback(int32_t* width, int32_t* height) { SQUE_PRINT(LT_INFO, "Get Viewport Size Callback not Set!"); }
ViewportSizeCallback viewport_size_callback = DebugViewportSizeCallback;

static int32_t def_window_options[] =
{
    SQUE_DISPLAY_END
};

static int32_t const def_context_options[] =
{
    SQUE_WINDOW_CONTEXT_MAJOR, SQUE_CONTEXT_MAJOR_MIN,
    SQUE_DISPLAY_END
};

static int32_t const def_buffer_options[] =
{
    SQUE_RED_BITS, 8,
    SQUE_GREEN_BITS, 8,
    SQUE_BLUE_BITS, 8,
    SQUE_ALPHA_BITS, 8,
    SQUE_BUFFER_SIZE, 32,
    SQUE_STENCIL_BITS, 0,
    SQUE_DEPTH_BITS, 24,
    SQUE_RENDERABLE_TYPE, SQUE_MIN_RENDERABLE,
    SQUE_DISPLAY_END
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
        SQUE_PRINT(SQUE_LogType::LT_ERROR, "GLFW_ERROR-%d: %s", error_code, description);
    }

    void GLFW_CloseCallback(GLFWwindow* window)
    {
        for (int i = 0; i < glfw_windows.size(); ++i)
            if (glfw_windows[i] == window) SQUE_DISPLAY_SetWindowClose(i);
    }

    int32_t GetSQUE_WindowFromGLFWwindow(GLFWwindow* window)
    {
        SQUE_Window* ret = nullptr;
        for(int i = 0; i < sque_windows.size(); ++i)
        {
            if(glfw_windows[i] == window)
                return i;
        }
        return -1;
    }

    void GLFW_FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        int32_t win = GetSQUE_WindowFromGLFWwindow(window);   
        if (win < 0 || win >= sque_windows.size())
        {
            SQUE_PRINT(LT_WARNING, "Rogue GLFW window!");
            return;
        }
        SQUE_DISPLAY_ResizeWindow(win, width, height);
        
        SQUE_PRINT(SQUE_LogType::LT_INFO, "Resized Window \"%s\": %d %d", sque_windows[win]->title, width, height);    
    }

    static void GLFW_MouseEnterLeaveCallback(GLFWwindow* window, int entered)
    {
        for (int i = 0; i < glfw_windows.size(); ++i)
            if (glfw_windows[i] == window)
            {
                (entered > 0) ? SET_FLAG(sque_windows[i]->working_flags, SQUE_WINDOW_MOUSE_IN) : CLR_FLAG(sque_windows[i]->working_flags, SQUE_WINDOW_MOUSE_IN);
                break;
            }
    }

#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZING & STATE MANAGEMENT ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_DISPLAY_Init()
{
    // What if some dipshit calls an Init 2 times?
    // SHould I have a callback that is dereferenced?
    // Dereference would throw an error immediately which is good for people to learn
    // But some things and modules require to be reinitialized after coming back...
    // Should I have a bool?
    // think about it.
    if (!next_window)
    {
        SQUE_PRINT(SQUE_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new SQUE_Window();
        // Default window hints...
        next_window->buffer_options = (int32_t*)def_buffer_options;
        next_window->context_options = (int32_t*)def_context_options;
        next_window->context_options = (int32_t*)def_window_options;
    }

    SQUE_PRINT(LT_INFO, "Declaring Backed Specific Variables...");

    // BackEnd Specific Variables
#if defined(USE_EGL)
    EGLint egl_major, egl_minor;
#elif defined USE_GLFW
    int glfw_major, glfw_minor, glfw_revision;
    glfwSetErrorCallback(GLFW_ErrorCallback);
#endif

#if defined(ANDROID)
    SQUE_PRINT(LT_INFO, "ANDROID - Waiting of Graphics Backend Initialization...");
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

    SQUE_PRINT(LT_INFO, "Initializing Display Backend...");
// Backend Initialization
#if defined(USE_EGL)
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(egl_display == EGL_NO_DISPLAY)
    {
        SQUE_PRINT(SQUE_LogType::LT_ERROR, "EGL Found no Display!");
        exit(0);
    }
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Found main display with EGL!");

    if(!eglInitialize(egl_display, &egl_major, &egl_minor))
    {
        SQUE_PRINT(SQUE_LogType::LT_ERROR, "EGL failed to Initialize!");
        exit(0);
    }
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Successfully Initialized EGL!");

    SQUE_PRINT(SQUE_LogType::LT_INFO, "EGL_VERSION: \"%s\" \nEGL_VENDOR: \"%s\"\nEGL_EXTENSIONS: \"%s\"",
        eglQueryString(egl_display, EGL_VERSION), 
        eglQueryString(egl_display, EGL_VENDOR), 
        eglQueryString(egl_display, EGL_EXTENSIONS));

    EGLConfig config;
    EGLint num_config;

    SQUE_PRINT(SQUE_LogType::LT_INFO, "Preparing Config...");
    eglChooseConfig(egl_display, def_buffer_options, &config, 1, &num_config);
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Using EGL Config %d", num_config);

    SQUE_PRINT(SQUE_LogType::LT_INFO, "Creating EGL Context...");

    egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, def_context_options);
    if (egl_context == EGL_NO_CONTEXT)
    {
        SQUE_PRINT(SQUE_LogType::LT_ERROR, "Could not create EGL Context!");
        exit(0);
    }
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Created EGL Context!");

    if (egl_window)
    {
#if defined(ANDROID)
        if (!my_app->window)
        {
            SQUE_PRINT(SQUE_LogType::LT_WARNING, "App restarted without creating window, stopping...");
            exit(0);
        }
#endif
    }
#if defined(ANDROID)
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Creating EGL window = surface");
    egl_window = my_app->window;
#endif
    if (!egl_window)
    {
        SQUE_PRINT(SQUE_LogType::LT_ERROR, "EGL Window got dereferenced!");
        exit(0);
    }

#if defined(ANDROID)
    int32_t width = ANativeWindow_getWidth(egl_window);
    int32_t height = ANativeWindow_getHeight(egl_window);
    next_window->width = width;
    next_window->height = height;
    EGLSurface test = eglCreateWindowSurface(egl_display, config, egl_window, def_window_options);
    SQUE_PRINT(SQUE_LogType::LT_INFO, "ANDROID - Surface Size: %d %d", width, height);
#else // Other EGL based backends?
#endif
    
    
    if (test == EGL_NO_SURFACE)
    {
        if(egl_surface == EGL_NO_SURFACE)
        {
        SQUE_PRINT(SQUE_LogType::LT_ERROR, "Failed to create EGL Surface!: %#x", eglGetError());
        exit(0);
        }
    }
    else
        egl_surface = test;

    SQUE_PRINT(SQUE_LogType::LT_INFO, "Created EGL Surface!");

#elif defined(USE_GLFW)
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Compiled with GLFW Version: %d.%d.%d", GLFW_VERSION_MAJOR, GLFW_VERSION_MINOR, GLFW_VERSION_REVISION);
    glfwGetVersion(&glfw_major, &glfw_minor, &glfw_revision);
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Using GLFW Version: %d.%d.%d", glfw_major, glfw_minor, glfw_revision);

// GLFW Hints - Before calling glfwInit(), setup behaviour.
    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_TRUE); // It is default value but for test and check
    if (!glfwInit()) exit(0);
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Successfully Initialized GLFW!");

    glfw_monitors = glfwGetMonitors(&monitor_count); // Main Monitor is always 0
    //glfwSetMonitorCallback( _send_monitor_change_event_ );
#endif

}

void SQUE_DISPLAY_Close()
{
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Closing SQUE_Displays...");
#if defined(USE_EGL)

#elif defined USE_GLFW
    uint16_t glfw_size = glfw_windows.size();
    for (int i = 0; i < glfw_size; ++i)
        glfwDestroyWindow(glfw_windows[i]);
    glfw_windows.clear();
    glfwTerminate();
#endif
    uint16_t size = sque_windows.size();
    for (int i = 0; i < size; ++i)
        delete sque_windows[i];
    sque_windows.clear();
}

void SQUE_DISPLAY_SetVSYNC(int16_t vsync_val)
{
    if (main_window_context == UINT16_MAX)
    {
        SQUE_PRINT(SQUE_LogType::LT_WARNING, "No main context to act on, dismissed change of SwapInterval!");
        return;
    }
#if defined(USE_EGL)
    // should pass monitor uint to change refresh rate
    eglSwapInterval(egl_display, vsync_val);
#elif defined USE_GLFW
    glfwSwapInterval(vsync_val);
#endif
}

int32_t SQUE_DISPLAY_GetDPIDensity(uint16_t window)
{
#if defined(ANDROID)
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, my_app->activity->assetManager);
    int32_t density = AConfiguration_getDensity(config);
    AConfiguration_delete(config);
    return density;
#elif defined (USE_GLFW)
    const GLFWvidmode* mode = glfwGetVideoMode(glfw_monitors[0]);
    int width_mm, height_mm;
    glfwGetMonitorPhysicalSize(glfw_monitors[0], &width_mm, &height_mm);
    float inches = (width_mm * height_mm) / 25.4f;

    return (mode->width * mode->height) / inches;
#endif

    return 400;

}

void SQUE_DISPLAY_GetMainDisplaySize(uint16_t& w, uint16_t& h)
{
#if defined(USE_GLFW)
    const GLFWvidmode* mode = glfwGetVideoMode(glfw_monitors[0]);
    w = mode->width;
    h = mode->height;
#elif defined(USE_EGL)
    w = sque_windows[0]->width;
    h = sque_windows[0]->height;
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROL SPECIFIC WINDOWS //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void* SQUE_DISPLAY_GetPlatformWindowHandle(uint16_t window)
{
#if defined(USE_GLFW)
#   if defined(_WIN32)
    return (void*)glfwGetWin32Window(glfw_windows[window]);
#   endif
#else
#endif

    return nullptr;
}

void SQUE_DISPLAY_ResizeWindow(uint16_t window, uint16_t w, uint16_t h)
{
    if(window >= sque_windows.size()) return;

    sque_windows[window]->width = w;
    sque_windows[window]->height = h;
#if defined(USE_GLFW)
    glfwSetWindowSize(glfw_windows[window], w, h);
#elif defined(USE_EGL)
#endif
    uint16_t prev_main = main_window_context;
    SQUE_DISPLAY_MakeContextMain(window);
    viewport_resize_callback(w,h);
    SQUE_DISPLAY_MakeContextMain(prev_main);
}

void SQUE_DISPLAY_UpdateNativeWindowSize(uint16_t window)
{
    int32_t w, h;
#if defined(USE_EGL)
    w = ANativeWindow_getWidth(egl_window);
    h = ANativeWindow_getHeight(egl_window);
#else if defined(USE_GLFW)
    glfwGetWindowSize(glfw_windows[window], &w, &h);
#endif

    SQUE_DISPLAY_ResizeWindow(window, w,h);
}
void SQUE_DISPLAY_GetWindowSize(uint16_t window, int32_t* x, int32_t* y)
{
    if (window < sque_windows.size())
    {
        *x = sque_windows[window]->width;
        *y = sque_windows[window]->height;
    }
    else
    {
        SQUE_PRINT(SQUE_LogType::LT_WARNING, "Window not available!");
    }
}

void SQUE_DISPLAY_GetViewportSize(uint16_t window, int32_t* w, int32_t* h)
{
    uint16_t prev_main = main_window_context;
    SQUE_DISPLAY_MakeContextMain(window);
    if (window < sque_windows.size())
        viewport_size_callback(w, h);
    else
        SQUE_PRINT(SQUE_LogType::LT_WARNING, "Window not available!");

    SQUE_DISPLAY_MakeContextMain(prev_main);
}


uint16_t SQUE_DISPLAY_GetAmountWindows() { return sque_windows.size(); }

void SQUE_DISPLAY_SetWindowClose(uint16_t window)
{
    if (window < sque_windows.size())
        SET_FLAG(sque_windows[window]->working_flags, SQUE_WINDOW_TO_CLOSE);
    else
        SQUE_PRINT(LT_WARNING, "Invalid window...");
}

bool SQUE_DISPLAY_ShouldWindowClose(uint16_t window)
{
    if(window < sque_windows.size())
        return CHK_FLAG(sque_windows[window]->working_flags, SQUE_WINDOW_TO_CLOSE);
    return false;
}

uint16_t SQUE_DISPLAY_CloseWindow(uint16_t window)
{
    uint16_t size = sque_windows.size();
    if(window < size)
    {
        SQUE_PRINT(SQUE_LogType::LT_INFO, "Closing window n*%d with title %s", window, sque_windows[window]->title);
        delete sque_windows[window];
        sque_windows[window] = sque_windows[size-1];
        sque_windows.pop_back();

#if defined(USE_EGL)

#elif defined(USE_GLFW)
        glfwDestroyWindow(glfw_windows[window]);
        glfw_windows[window] = glfw_windows[size - 1]; // This changes the order drastically, but fast
        glfw_windows.pop_back();
#endif
        SQUE_PRINT(SQUE_LogType::LT_INFO, "Window n*%d with title %s is now n*%d", size-1, sque_windows[window]->title, window);
    }
    else
    {
        SQUE_PRINT(SQUE_LogType::LT_WARNING, "Window n*%d not available!");
    }
    return window; // When a window is popped that is not last, last becomes this window, thus return position of previous last window
}

void SQUE_DISPLAY_NextWindow_WindowHints(int32_t* options, int32_t size)
{
    if (!next_window)
    {
        SQUE_LOG(SQUE_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new SQUE_Window();
    }

    next_window->window_options = options;
    next_window->num_window_options = size;
}

void SQUE_DISPLAY_NextWindow_ContextHints(int32_t* options, int32_t size)
{
    if (!next_window)
    {
        SQUE_LOG(SQUE_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new SQUE_Window();
    }

    next_window->context_options = options;
    next_window->num_context_options = size;
}

void SQUE_DISPLAY_NextWindow_BufferHints(int32_t* options, int32_t size)
{
    if (!next_window)
    {
        SQUE_LOG(SQUE_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new SQUE_Window();
    }

    next_window->buffer_options = options;
    next_window->num_buffer_options = size;
}


void SQUE_DISPLAY_OpenWindow(const char* title, int32_t width, int32_t height, uint16_t monitor)
{
#if defined(ANDROID)
    if(sque_windows.size() > 0)
    {
        SQUE_PRINT(LT_WARNING, "On Android I am not supporting multiple windows currently...");
        return;
    }
#endif

    // Set to monitor[0] for fullscreen at 4th parameter
    if (!next_window)
    {
        SQUE_PRINT(SQUE_LogType::LT_WARNING, "No window hints set, creating with defaults...");
        next_window = new SQUE_Window();
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
        SQUE_PRINT(LT_WARNING, "Unable to create GLFW window...");
        return;
    }
    glfw_windows.push_back(glfw_window);
    glfwSetWindowCloseCallback(glfw_window, GLFW_CloseCallback);
    glfwSetFramebufferSizeCallback(glfw_window, GLFW_FramebufferResizeCallback);
    glfwSetCursorEnterCallback(glfw_window, GLFW_MouseEnterLeaveCallback);
    SQUE_PRINT(SQUE_LogType::LT_INFO, "GLFW Window Created!");
#endif

    SQUE_PRINT(SQUE_LogType::LT_INFO, "Window \"%s\" opened correctly", title);
#ifndef ANDROID // Quick and dirty, will look into initing properly
    next_window->width = width;
    next_window->height = height;
#endif
    sque_windows.push_back(next_window);
    SQUE_DISPLAY_MakeContextMain(sque_windows.size()-1);
    next_window = NULL;
}

void SQUE_DISPLAY_GetWindowPos(uint16_t window, int32_t& x, int32_t& y)
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

void SQUE_DISPLAY_SwapBuffer(uint16_t window)
{
    if(window > sque_windows.size())
    {
        SQUE_PRINT(LT_WARNING, "Out of range window...");
        return;
    }
#if defined(USE_EGL)
    // Revise when i get a multiple window frontend for egl
    eglSwapBuffers(egl_display, egl_surface);
#elif defined USE_GLFW
    glfwSwapBuffers(glfw_windows[window]);
#endif
}

void SQUE_DISPLAY_SwapAllBuffers()
{
    for(int i = 0; i < sque_windows.size(); ++i)
    {   
        SQUE_DISPLAY_MakeContextMain(i);
#if defined(USE_EGL)
        eglSwapBuffers(egl_display, egl_surface);
        sque_windows[i]->width = ANativeWindow_getWidth(egl_window);
        sque_windows[i]->height = ANativeWindow_getHeight(egl_window);
        return;
#elif defined(USE_GLFW)
        glfwSwapBuffers(glfw_windows[i]);
#endif
    }
}

void SQUE_DISPLAY_MakeContextMain(uint16_t window)
{
    if (window > sque_windows.size())
    {
        SQUE_PRINT(SQUE_LogType::LT_WARNING, "Invalid context!");
        return;
    }
#if defined(USE_EGL)
    if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context))
    {
        SQUE_PRINT(SQUE_LogType::LT_ERROR, "Failed to attach Context to Surface (eglMakeCurrent())!");
        exit(0);
    };
#elif defined(USE_GLFW)
    glfwMakeContextCurrent(glfw_windows[window]);
#endif
    main_window_context = window;
}

ResizeCallback SQUE_DISPLAY_SetViewportResizeCallback(ResizeCallback viewport_cb)
{
    ResizeCallback ret = viewport_resize_callback;
    viewport_resize_callback = viewport_cb;
    return ret;
}

ViewportSizeCallback SQUE_DISPLAY_SetViewportSizeCallback(ViewportSizeCallback viewport_size_cb)
{
    ViewportSizeCallback ret = viewport_size_callback;
    viewport_size_callback = viewport_size_cb;
    return ret;
}