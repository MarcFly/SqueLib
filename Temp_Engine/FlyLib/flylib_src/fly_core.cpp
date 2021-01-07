#include "fly_lib.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CORE LIBRARY FUNCTIONS ////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


bool FLYLIB_Init(/* flags */)
{
    bool ret = true;

    // Call Init for all loaded modules and with required flags
    FLYLOGGER_Init(/*pass flag true or false*/ true);
    FLYDISPLAY_Init(FLYWINDOW_MAXIMIZED, "FlyLib Test Window");
    FLYRENDER_Init();
    return ret;
}

bool FLYLIB_Close(/* flags */)
{
    bool ret = true;

    // Call close for all required modules for FlyLib

    FLYRENDER_Close();
    FLYINPUT_Close();

    // Display is last that has ties with GLFW, holds GLFWTerminate
    FLYDISPLAY_Close(); 

    // Unrelated Helpers that are still required
    FLYLOGGER_Close();

    return ret;
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

#ifdef ANDROID

// Putting the android entry point under window module because in
// android you acquire a native_app as if it were the window to act upon.
#include <android_native_app_glue.h>

extern void HandleAndroidCMD(struct android_app* app, int32_t cmd);
extern int32_t HandleAndroidInput(struct android_app* app, AInputEvent* ev);
extern int main();
struct android_app* app;
void android_main(struct android_app* gapp)
{
    app = gapp;
    #ifndef FLYLOGGER_OUT
    FLY_ConsolePrint(FLY_LogType::LT_INFO, "FLYLIB - Android Flylib Start...");
    #endif

    app->onAppCmd = HandleAndroidCMD;
    app->onInputEvent = HandleAndroidInput;
    //char *argv[] = {"AppMain", 0};
    FLY_ConsolePrint(FLY_LogType::LT_INFO, "FLYLIB - Calling App Main...");
    main();
    //app->destroyRequested = 0;
    FLY_ConsolePrint(FLY_LogType::LT_INFO, "FLYLIB - Finished executing App...");
}
#endif