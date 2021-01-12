
#ifdef DISPLAY_SOLO
#   include "fly_display.h"
#else
#   include "fly_lib.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DISPLAY / WINDOW MANAGEMENT ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define which API Backend to USE
#ifdef ANDROID
    extern int OGLESStarted;
    extern struct android_app* my_app;
#   include <android/native_activity.h>
#   include <android_native_app_glue.h>
#elif defined (_WIN32) || defined (__linux__)

#endif

// Platfrom Agnostic Includes and Variables
#include <vector>
#include <mutex>
static std::mutex display_mtx;
std::vector<FLY_Window*> fly_windows;
static uint16 main_window_context = UINT16_MAX;

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

#elif defined USE_GLFW
#   include <GLFW/glfw3.h>

    int monitor_count;
    GLFWmonitor** glfw_monitors;
    std::vector<GLFWwindow*> glfw_windows;
    
    void GLFW_ErrorCallback(int error_code, const char* description)
    {
        FLYLOG(FLY_LogType::LT_ERROR, "GLFW_ERROR-%d: %s", error_code, description);
    }

    FLY_Window* GetFLY_WindowFromGLFWwindow(GLFWwindow* window)
    {
        FLY_Window* ret = nullptr;
        for(int i = 0; i < fly_windows.size(); ++i)
        {
            if(glfw_windows[i] == window)
                return fly_windows[i];
        }
        return nullptr;
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
    
    void GLFW_CloseCallback(GLFWwindow* window)
    {
        for(int i = 0; i < glfw_windows.size(); ++i)
            if(glfw_windows[i] == window) FLYDISPLAY_SetWindowClose(i);
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
// INITIALIZING & GLOBAL STATE CONTROL ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
bool FLYDISPLAY_Init(uint16 flags, const char* title, uint16 w, uint16 h)
{
    FLYLOG(LT_INFO, "Initializing Main Display...");
    bool ret = true;

    FLY_Window* init_window = new FLY_Window();
    init_window->title = title;
    init_window->width = w;
    init_window->height = h;
    SET_FLAG(init_window->flags,flags);
    fly_windows.push_back(init_window);

FLYLOG(LT_INFO, "Declaring Backed Specific Variables...");
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

    FLYLOG(LT_INFO, "Performing Platform Specific PreInitialization...");
// Platform Specifics PreInitialization
#ifdef ANDROID
    FLYLOG(LT_INFO, "ANDROID - Waiting of OpenGLES Initialization...");
    int events;
    while(!OGLESStarted)
    {  
        struct android_poll_source* source;
        if(ALooper_pollAll(0,0,&events, (void**)&source) >= 0)
        {
            if(source != NULL)
            {
                FLYLOG(LT_INFO, "Processing Init Inputs: Source %d, App %d", source, my_app);
                source->process(my_app, source);
            }
        }
             
    }
#endif

    FLYLOG(LT_INFO, "Initializing Display Backend...");
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
    
    FLYLOG(FLY_LogType::LT_INFO, "Preparing Config...");
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
        return false;
    }

#ifdef ANDROID
    init_window->width = ANativeWindow_getWidth(egl_window);
    init_window->height= ANativeWindow_getHeight(egl_window);
    egl_surface = eglCreateWindowSurface(egl_display, config, my_app->window, window_attribute_list);
    FLYLOG(FLY_LogType::LT_INFO, "ANDROID - Surface Size: %d %d", init_window->width, init_window->height);
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
    FLYDISPLAY_MakeContextMain(0);
    FLYDISPLAY_SetVSYNC(1); // Swap Interval
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

#ifdef ANDROID
#include <android/configuration.h>
#endif
int32 FLYDISPLAY_GetDPIDensity(uint16 window)
{
#ifdef ANDROID
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROL SPECIFIC WINDOWS //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLYDISPLAY_Resize(uint16 window, uint16 w, uint16 h)
{
    if(window >= fly_windows.size()) return;

    fly_windows[window]->width = w;
    fly_windows[window]->height = h;
#ifdef USE_GLFW
    //glfwSetWindowSize(glfw_windows[window], w, h);
#elif defined USE_EGL

#endif

}

void FLYDISPLAY_GetSize(uint16 window, uint16* x, uint16* y)
{
    uint16 size = fly_windows.size();
    if (size > window && x != NULL & y != NULL)
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

void FLYDISPLAY_SetWindowClose(uint16 window)
{
    if(window < fly_windows.size())
        SET_FLAG(fly_windows[window]->flags, FLYWINDOW_TO_CLOSE);
}

bool FLYDISPLAY_ShouldWindowClose(uint16 window)
{
    if(window < fly_windows.size())
        return (fly_windows[window]->flags & FLYWINDOW_TO_CLOSE) > 0;
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

#ifdef USE_GLFW
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

    int x=0,y=0,w=0,h=0;
    
#ifdef USE_EGL
    eglQuerySurface(egl_display, egl_surface, EGL_WIDTH, &w);
    eglQuerySurface(egl_display, egl_surface, EGL_HEIGHT, &h);
    window->width = w;
    window->height = h;

#elif defined USE_GLFW
    glfwGetMonitorWorkarea(glfw_monitors[monitor], &x, &y, &w, &h);
    window->width = (window->width != 0) ? window->width : w;
    window->height = (window->height != 0) ? window->height : h;

#endif

    FLYDISPLAY_NextWindowOptions(window->flags);
    // hardcoded window setup for working with opengl or opengles
#ifdef USE_GLFW
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
        );
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#elif defined USE_EGL
#endif

#ifdef USE_EGL

#elif defined USE_GLFW
    GLFWwindow* glfw_window = glfwCreateWindow(w, h, window->title, NULL, NULL);
    if(!glfw_window) return false;
    glfw_windows.push_back(glfw_window);
    glfwSetWindowCloseCallback(glfw_window, GLFW_CloseCallback);
    glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);
    FLYLOG(FLY_LogType::LT_INFO, "GLFW Window Created!");
#endif
    FLYLOG(FLY_LogType::LT_INFO, "FLY_Window Created!");
    


    FLYLOG(FLY_LogType::LT_INFO, "Window \"%s\" opened correctly", window->title);

    FLYDISPLAY_MakeContextMain(fly_windows.size()-1);
    FLYINPUT_Init(fly_windows.size() - 1);

    return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONTROLLING CONTEXTS //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLYDISPLAY_CleanAll()
{
    
}

void FLYDISPLAY_SwapAllBuffers()
{
    if(fly_windows.size() < 1) return;
#ifdef USE_EGL
    eglSwapBuffers(egl_display, egl_surface);
    fly_windows[0]->width = ANativeWindow_getWidth( egl_window );
	fly_windows[0]->height = ANativeWindow_getHeight( egl_window );
	FLYRENDER_ChangeViewPortSize(fly_windows[0]->width, fly_windows[0]->height);
#endif


    for(int i = 0; i < fly_windows.size(); ++i)
    {        
#ifdef USE_GLFW
        glfwSwapBuffers(glfw_windows[i]);
#endif
    }
}

void FLYDISPLAY_SwapBuffers(uint16 window)
{
#ifdef USE_EGL
    eglSwapBuffers(egl_display, egl_surface);
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
    main_window_context = 0;
#ifdef USE_EGL
#elif defined USE_GLFW
    glfwMakeContextCurrent(glfw_windows[window]);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZING & GLOBAL STATE CONTROL ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef USE_GLFW
struct GLFWwindow;
#endif
GLFWwindow* FLYDISPLAY_RetrieveMainGLFWwindow()
{
#ifndef USE_GLFW
    return nullptr;
#else
    return glfw_windows[0];
#endif
}

#ifndef USE_EGL
void* eglGetProcAddress(const char*) {return nullptr;}
#endif
/*void(*FLYDISPLAY_RetrieveEGLProcAddress())(const char*)
{
    return eglGetProcAddress;
}*/
