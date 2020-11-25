#include <android_native_app_glue.h>
#include <unordered_map>

void handle_android_cmd(struct android_app* app, int32_t cmd)
{

}

int32_t handle_android_input(struct android_app* app, AInputEvent* ev)
{
    // Return 1 for event handled, 0 for default
    // Probably in my Engine I will pass the inputs to the main input handler
    // in the according type and create a callback function for the post_exec
    // thus this will probably never finish the handling of an event but sending
    // the events to engine and engine to post_exec them
    return 0;
}

void android_main(struct android_app* app)
{

    app->onAppCmd = handle_android_cmd;
    app->onInputEvent = handle_android_input;

    app->destroyRequested = 0;
}