
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
static std::mutex display_mtx;
std::vector<FLY_Window*> fly_windows;
static uint16 main_window_context = UINT16_MAX;
int monitor_count;

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

#if defined(USE_EGL)
#   include <EGL/egl.h>
    EGLNativeWindowType egl_window;
    EGLDisplay egl_display;
    EGLSurface egl_surface;
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

    // These should be part of the renderer (even if it is glfw, because they affect the framebuffer
    FLY_Window* GetFLY_WindowFromGLFWwindow(GLFWwindow* window)
    {
        FLY_Window* ret = nullptr;
        for(int i = 0; i < fly_windows.size(); ++i)
        {
            if(glfw_windows[i] == window)
                return fly_windows[i];
        }
        return ret;
    }

    void GLFW_FramebufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        FLY_Window* win = GetFLY_WindowFromGLFWwindow(window);
        if(win == nullptr) FLYLOG(FLY_LogType::LT_WARNING, "Did not find FLY_Window attached to GLFWwindow...");

        win->width = width;
        win->height = height;
        // TODO: Create and Call render Resize Viewport
        FLYLOG(FLY_LogType::LT_INFO, "Resized Window \"%s\": %d %d", width, height);
    
    }
#endif



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
// INITIALIZING & STATE MANAGEMENT ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLYDISPLAY_Init(uint16 flags, const char* title, uint16 w, uint16 h)
{
    FLYLOG(LT_INFO, "Initializing Main Display...");

    FLY_Window* init_window = new FLY_Window();
    init_window->title = title;
    init_window->width = w;
    init_window->height = h;
    SET_FLAG(init_window->flags,flags);
    fly_windows.push_back(init_window);

    FLYLOG(LT_INFO, "Declaring Backed Specific Variables...");

    // BackEnd Specific Variables
#if defined(USE_EGL)
    EGLint egl_major, egl_minor;
    EGLConfig config;
    EGLint num_config;
    EGLContext egl_context;
#elif defined USE_GLFW
    int glfw_major, glfw_minor, glfw_revision;
    glfwSetErrorCallback(GLFW_ErrorCallback);
#endif


    FLYLOG(LT_INFO, "Initializing Display Backend...");
// Backend Initialization
#if defined(USE_EGL)
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(egl_display == EGL_NO_DISPLAY)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "EGL Found no Display!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Found main display with EGL!");

    if(!eglInitialize(egl_display, &egl_major, &egl_minor))
    {
        FLYLOG(FLY_LogType::LT_ERROR, "EGL failed to Initialize!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Successfully Initialized EGL!");

    FLYLOG(FLY_LogType::LT_INFO, "EGL_VERSION: \"%s\" \nEGL_VENDOR: \"%s\"\nEGL_EXTENSIONS: \"%s\"",
        eglQueryString(egl_display, EGL_VERSION), 
        eglQueryString(egl_display, EGL_VENDOR), 
        eglQueryString(egl_display, EGL_EXTENSIONS));
    
    FLYLOG(FLY_LogType::LT_INFO, "Preparing Config...");
    eglChooseConfig(egl_display, config_attribute_list, &config, 1, &num_config);
    FLYLOG(FLY_LogType::LT_INFO, "Using EGL Config %d", num_config);

    FLYLOG(FLY_LogType::LT_INFO, "Creating EGL Context...");
    egl_context = eglCreateContext(egl_display, config, EGL_NO_CONTEXT, context_attribute_list);
    if(egl_context == EGL_NO_CONTEXT)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Could not create EGL Context!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Created EGL Context!");

#if defined(ANDROID)
    if( egl_window && !my_app->window)
    {
        FLYLOG(FLY_LogType::LT_WARNING, "App restarted without creating window, stopping...");
        exit(0);
    }
#endif

    FLYLOG(FLY_LogType::LT_INFO, "Getting EGL Surface..");
    egl_window = my_app->window;
    if(!egl_window)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "EGL Window got dereferenced!");
        exit(0);
    }

#if defined(ANDROID)
    init_window->width = ANativeWindow_getWidth(egl_window);
    init_window->height= ANativeWindow_getHeight(egl_window);
    egl_surface = eglCreateWindowSurface(egl_display, config, my_app->window, window_attribute_list);
    FLYLOG(FLY_LogType::LT_INFO, "ANDROID - Surface Size: %d %d", init_window->width, init_window->height);
#else // Other EGL based backends?
#endif

    if(egl_surface == EGL_NO_SURFACE)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "Failed to create EGL Surface!");
        exit(0);
    }
    FLYLOG(FLY_LogType::LT_INFO, "Created EGL Surface!");

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

    if (!FLYDISPLAY_OpenWindow(init_window)) exit(0);
    FLYLOG(FLY_LogType::LT_INFO, "Opened main GLFW window!");
#endif

    FLYDISPLAY_MakeContextMain(0);
    FLYDISPLAY_SetVSYNC(1); // Swap Interval
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

void FLYDISPLAY_SetVSYNC(int16_t vsync_val)
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

#if defined(ANDROID)

#endif
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
void FLYDISPLAY_Resize(uint16 window, uint16 w, uint16 h)
{
    if(window >= fly_windows.size()) return;

    fly_windows[window]->width = w;
    fly_windows[window]->height = h;
#if defined(USE_GLFW)
    glfwSetWindowSize(glfw_windows[window], w, h);
#elif defined(USE_EGL)
    // ?
#endif
    // Maybe create a callback system for these types of functions that are automatically called
    // But maybe, they are supposed to be called by other backends if not use ours
    // FLYRENDER_ChangeViewPortSize(w, h);
}

// Unsure if this returns content rect or actual window size on GLFW, because it resizes to content rect I think
void FLYDISPLAY_GetWindowSize(uint16 window, uint16* x, uint16* y)
{
    uint16 size = fly_windows.size();
    if (size > window && x != NULL && y != NULL)
    {
        *x = fly_windows[window]->width;
        *y = fly_windows[window]->height;
    }
    else
    {
        FLYLOG(FLY_LogType::LT_WARNING, "Window not available!");
    }
}

uint16 FLYDISPLAY_GetAmountWindows() { return fly_windows.size(); }

void FLYDISPLAY_SetWindowClose(uint16 window)
{
    if (window < fly_windows.size())
        SET_FLAG(fly_windows[window]->flags, FLYWINDOW_TO_CLOSE);
    else
        FLYLOG(LT_WARNING, "Invalid window...");
}

bool FLYDISPLAY_ShouldWindowClose(uint16 window)
{
    if(window < fly_windows.size())
        return CHK_FLAG(fly_windows[window]->flags, FLYWINDOW_TO_CLOSE);
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

void FLYDISPLAY_NextWindowOptions(uint16 flags)
{
#if defined(USE_EGL)
    // Here use the flags to save the array of options!!
#elif defined(USE_GLFW)
    (flags & FLYWINDOW_NOT_RESIZABLE) ? glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE) : glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    (flags & FLYWINDOW_NOT_DECORATED) ? glfwWindowHint(GLFW_DECORATED, GLFW_FALSE) : glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    (flags & FLYWINDOW_ALWAYS_TOP) ? glfwWindowHint(GLFW_FLOATING, GLFW_TRUE) : glfwWindowHint(GLFW_FLOATING, GLFW_FALSE);
    (flags & FLYWINDOW_MAXIMIZED) ? glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE) : glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE);


    // Private Options
    //glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    //glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_NATIVE_CONTEXT_API);
#endif
}

#if defined(USE_GLFW)
extern void framebuffer_size_callback(GLFWwindow* window, int width, int height);
#endif

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

    FLYDISPLAY_NextWindowOptions(window->flags);

    int x = 0, y = 0, w = 0, h = 0;

#if defined(USE_EGL)
    eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &w);
    eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &h);
    window->width = w;
    window->height = h;

#elif defined(USE_GLFW)
    glfwGetMonitorWorkarea(glfw_monitors[monitor], &x, &y, &w, &h);
    window->width = (window->width != 0) ? window->width : w;
    window->height = (window->height != 0) ? window->height : h;
    FLYPRINT(LT_INFO, "Test %d,%d",w,h);
#ifndef __linux__
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#else
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#endif
    GLFWwindow* glfw_window = glfwCreateWindow(w, h, window->title, NULL, NULL);
    glfwGetFramebufferSize(glfw_window, &w, &h);
    FLYPRINT(LT_INFO, "Test %d,%d",w,h);
    glfwGetWindowSize(glfw_window, &w, &h);
    FLYPRINT(LT_INFO, "Test %d,%d",w,h);
    window->width = w;
    window->height = h;
    if (!glfw_window) return false;
    glfw_windows.push_back(glfw_window);
    glfwSetWindowCloseCallback(glfw_window, GLFW_CloseCallback);
    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
    FLYLOG(FLY_LogType::LT_INFO, "GLFW Window Created!");
#endif

    FLYLOG(FLY_LogType::LT_INFO, "Window \"%s\" opened correctly", window->title);

    FLYDISPLAY_MakeContextMain(fly_windows.size()-1);
    FLYINPUT_Init(fly_windows.size() - 1);

    return true;
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
        int x=0,y=0;
        FLYDISPLAY_GetWindowPos(i, x, y);
        FLYRENDER_ChangeViewPortSize(x,y,fly_windows[i]->width, fly_windows[i]->height);
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