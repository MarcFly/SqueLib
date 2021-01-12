#include "fly_lib.h"


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DEFINITION ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

VoidFun on_resume_callback = NULL;
VoidFun on_go_background_callback = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLATFORM SPECIFICS ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ANDROID

// Putting the android entry point under window module because in
// android you acquire a native_app as if it were the window to act upon.
#include <android_native_app_glue.h>

int OGLESStarted = 0;
struct android_app* my_app;

void HandleAndroidCMD(struct android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_INIT_WINDOW");
        my_app = app;
        if (!OGLESStarted) OGLESStarted = 1;
        else
        {
            FLYLIB_Init();
            on_resume_callback();
        }
        break;
    case APP_CMD_TERM_WINDOW:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_TERM_WINDOW");
        break;
    case APP_CMD_WINDOW_RESIZED:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_WINDOW_RESIZED");
        break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_WINDOW_REDRAW_NEEDED");
        break;
    case APP_CMD_CONTENT_RECT_CHANGED:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_CONTENT_RECT_CHANGED");
        break;
    case APP_CMD_GAINED_FOCUS:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_GAINED_FOCUS");
        break;
    case APP_CMD_LOST_FOCUS:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_LOST_FOCUS");
        break;
    case APP_CMD_CONFIG_CHANGED:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_CONFIG_CHANGED");
        break;
    case APP_CMD_LOW_MEMORY:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_LOW_MEMORY");
        break;
    case APP_CMD_START:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_START");
        break;
    case APP_CMD_RESUME:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_RESUME");
        break;
    case APP_CMD_SAVE_STATE:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_SAVE_STATE");
        on_go_background_callback();
        break;
    case APP_CMD_PAUSE:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_PAUSE");
        // Register Go To Background functions and call them
        break;
    case APP_CMD_STOP:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_STOP");
        break;
    case APP_CMD_DESTROY:
        FLYPRINT(FLY_LogType::LT_INFO, "APP_CMD_DESTROY");
        FLYDISPLAY_SetWindowClose(0); // Set to Close main window and end execution
        break;
    }
}

extern int32_t HandleAndroidInput(struct android_app* app, AInputEvent* ev);

// On Android, This main is first call that will call your defined main
extern int main(int argc, char** argv);
void android_main(struct android_app* app)
{
    my_app = app;
    #ifndef FLYLOGGER_OUT
    FLYPRINT(FLY_LogType::LT_INFO, "FLYLIB - Android Flylib Start...");
    #endif

    app->onAppCmd = HandleAndroidCMD;
    app->onInputEvent = HandleAndroidInput;
    char *argv[] = {"AppMain", 0};
    FLYPRINT(FLY_LogType::LT_INFO, "FLYLIB - Calling App Main...");
    main(2, argv);
    //app->destroyRequested = 0;
    FLYPRINT(FLY_LogType::LT_INFO, "FLYLIB - Finished executing App...");
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION AND STATE CONTROL ////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLYLIB_Init(/* flags */)
{
    // Call Init for all loaded modules and with required flags
    FLYLOGGER_Init(true);
    FLYDISPLAY_Init(FLYWINDOW_MAXIMIZED, "FlyLib Test Window");
    FLYRENDER_Init();
}

void FLYLIB_Close()
{
    // Call close for all required modules for FlyLib
    FLYRENDER_Close();
    FLYINPUT_Close();

    // Display is last that has ties with GLFW, holds GLFWTerminate
    FLYDISPLAY_Close(); 

    // Unrelated Helpers that are still required
    FLYLOGGER_Close();

#ifdef ANDROID
    ANativeActivity_finish(my_app->activity);
#endif
}
unsigned int FLGetVersion(void)
{
    return FL_VERSION;
}

int FLIsCompatibleDLL(void)
{
    uint32_t major = FLGetVersion() >> 16;
    return major == FL_VERSION_MAJOR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACKS FOR FLOW MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VoidFun FLYINPUT_AddOnResumeCallback(VoidFun fun)
{
    VoidFun ret = on_resume_callback;
    on_resume_callback = fun;
    return ret;

}
VoidFun FLYINPUT_AddOnGoBackgroundCallback(VoidFun fun)
{
    VoidFun ret = on_go_background_callback;
    on_go_background_callback = fun;
    return ret;
}