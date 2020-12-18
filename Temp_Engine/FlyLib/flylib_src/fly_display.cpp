
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
#endif

// Include the Necessary Backend for the chosen API
#ifdef USE_EGL
#   define EGL_ZBITS 16
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
        EGL_STENCIL_SIZE, 0, // ???
        EGL_DEPTH_SIZE, EGL_ZBITS,
#ifdef ANDROID
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
#else // ????????
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT | EGL_PIXMAP_BIT,
#endif 
        EGL_NONE
    };

    static EGLint window_attribute_list[] = {
        EGL_NONE
    };

    static const EGLint context_attribute_list[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

#   include <GLES3/gl32.h>
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

#ifdef USE_EGL
    EGLint egl_major, egl_minor;
    EGLConfig config;
    EGLint num_config;
    EGLContext egl_context;
#endif

#ifdef ANDROID
    int events;
    while(!OGLESStarted)
    {  
        struct android_poll_source* source;
        if(ALooper_pollAll(0,0,&events, (void**)&source) >= 0)
            if(source != NULL) source->process(app, source);
    }
#endif

#ifdef USE_EGL
    egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(egl_display == EGL_NO_DISPLAY)
    {
        FLYLOG(FlyLogType::LT_ERROR, "EGL Found no Display!");
        return false;
    }

    if(!eglInitialize(egl_display, &egl_major, &egl_minor))
    {
        FLYLOG(FlyLogType::LT_ERROR, "EGL failed to Initialize!");
        return false;
    }

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

#endif



    
    return ret;
}


bool FLYDISPLAY_Close()
{
    bool ret = true;

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