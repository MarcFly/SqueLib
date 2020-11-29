
#ifdef WINDOW_SOLO
#   include "fly_window.h"
#else
#   include "fly_lib.h"
#endif

#ifdef ANDROID

// Putting the android entry point under wundow module because in
// android you acquire a native_app as if it were the window to act upon.
#include <android_native_app_glue.h>

extern void handle_android_cmd(struct android_app* app, int32_t cmd);
extern int32_t handle_android_input(struct android_app* app, AInputEvent* ev);
extern int main();
void android_main(struct android_app* app)
{
    app->onAppCmd = handle_android_cmd;
    app->onInputEvent = handle_android_input;
    main();
    app->destroyRequested = 0;
}

#endif