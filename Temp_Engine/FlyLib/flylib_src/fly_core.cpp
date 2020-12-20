#include "fly_lib.h"

bool FLYLIB_Init(/* flags */)
{
    bool ret = true;

    // Call Init for all loaded modules and with required flags
    FLYLOGGER_Init(/*pass flag true or false*/ true);
    FLYDISPLAY_Init(FLYWINDOW_MAXIMIZED, "FlyLib Test Window", 400, 100);

    return ret;
}

bool FLYLIB_Close(/* flags */)
{
    bool ret = true;

    // Call close for all required modules for FlyLib
    FLYDISPLAY_Close();
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

extern void handle_android_cmd(struct android_app* app, int32_t cmd);
extern int32_t handle_android_input(struct android_app* app, AInputEvent* ev);
extern int main(int argc, char** argv);
void android_main(struct android_app* app)
{
    #ifndef FLYLOGGER_OUT
    FlyPrintLog("Android Flylib Start", FLY_LogType::LT_INFO);
    #endif

    app->onAppCmd = handle_android_cmd;
    app->onInputEvent = handle_android_input;
    main(1, {"AppMain"});
    app->destroyRequested = 0;
}
#endif