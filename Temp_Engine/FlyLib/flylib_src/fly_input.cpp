#ifdef INPUT_SOLO
#   include "fly_input.h"
#else
#   include "fly_lib.h"
#endif

#ifdef ANDROID

#include <android_native_app_glue.h>

int OGLESStarted = 0;
extern struct android_app* app;
void HandleAndroidCMD(struct android_app* app, int32_t cmd)
{
    switch(cmd)
    {
        case APP_CMD_INIT_WINDOW:
            FLYLOGGER_Print("APP_CMD_INIT_WINDOW", FLY_LogType::LT_INFO);
            if(!OGLESStarted) OGLESStarted = 1;
            break;
        case APP_CMD_TERM_WINDOW:
            FLYLOGGER_Print("APP_CMD_TERM_WINDOW", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_WINDOW_RESIZED:
            FLYLOGGER_Print("APP_CMD_WINDOW_RESIZED", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            FLYLOGGER_Print("APP_CMD_WINDOW_REDRAW_NEEDED", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            FLYLOGGER_Print("APP_CMD_CONTENT_RECT_CHANGED", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_GAINED_FOCUS:
            FLYLOGGER_Print("APP_CMD_GAINED_FOCUS", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_LOST_FOCUS:
            FLYLOGGER_Print("APP_CMD_LOST_FOCUS", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_CONFIG_CHANGED:
            FLYLOGGER_Print("APP_CMD_CONFIG_CHANGED", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_LOW_MEMORY:
            FLYLOGGER_Print("APP_CMD_LOW_MEMORY", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_START:
            FLYLOGGER_Print("APP_CMD_START", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_RESUME:
            FLYLOGGER_Print("APP_CMD_RESUME", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_SAVE_STATE:
            FLYLOGGER_Print("APP_CMD_SAVE_STATE", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_PAUSE:
            FLYLOGGER_Print("APP_CMD_PAUSE", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_STOP:
            FLYLOGGER_Print("APP_CMD_STOP", FLY_LogType::LT_INFO);
            break;
        case APP_CMD_DESTROY:
            FLYLOGGER_Print("APP_CMD_DESTROY", FLY_LogType::LT_INFO);
            FLYDISPLAY_SetWindowClose(0); // Set to Close main window and end execution
            break;
    } 
}

int32_t HandleAndroidMotion(struct android_app* app, AInputEvent* ev);

int32_t HandleAndroidInput(struct android_app* app, AInputEvent* ev)
{
    int32_t evtype = AInputEvent_getType(ev);
    int32_t ret;
    switch(evtype)
    {
        case AINPUT_EVENT_TYPE_FOCUS:
            break;
        case AINPUT_EVENT_TYPE_KEY:
            break;
        case AINPUT_EVENT_TYPE_MOTION:
            ret = HandleAndroidMotion(app, ev);
            break;
    }


    //int32 action = AInputEvent_getAction(ev);
    return 1;
}

#endif


void EmptyKeyCallback(const int prev_state, const int state) {}

typedef struct FLY_Key
{
    //int key;
    int prev_state = -1;
    int state = -1;
    void (*key_callback)(const int prev_state, const int state) = EmptyKeyCallback;
} FLY_Key;

typedef struct FLY_Mouse
{
    float prev_y, prev_x;
    float x, y;
    
    float prev_delta_y, prev_delta_x;
    
    double prev_scrollx, prev_scrolly;
    double scrollx, scrolly;

    FLY_Key mbuttons[16];
    
} FLY_Mouse;

FLY_Mouse mouse;
FLY_Key keyboard[512];



typedef struct FLY_Gesture
{
    FLY_Timer timer;

    float start_x, start_y;
    float midpoints[MAX_MIDPOINTS][2];
    float end_x, end_y;

    float refresh_bucket;
} FLY_Gesture;

typedef struct FLY_Pointer
{
    bool active = false;
    int32_t id;
    FLY_Gesture gesture;

} FLY_Pointer;

FLY_Pointer fly_pointers[MAX_POINTERS];

static void ResetPointers()
{
    for (int i = 0; i < MAX_POINTERS; ++i)
    {
        fly_pointers[i].active = false;
        fly_pointers[i].id = INT32_MAX;
        FLY_Gesture& g = fly_pointers[i].gesture;
        g.timer.Stop();
        g.start_x = 0;
        g.start_y = 0;
        g.end_x = 0;
        g.end_y = 0;
        g.refresh_bucket = 0;
        for (int j = 0; j < MAX_MIDPOINTS; ++j)
        {
            g.midpoints[j][0] = 0;
            g.midpoints[j][1] = 0;
        }
    }
}

static int GetPointer(int32_t id)
{
    int ret = 0;

    while (ret < 10)
    {
        if (!fly_pointers[ret].active || fly_pointers[ret].id == id)
        {
            fly_pointers[ret].id = id;
            fly_pointers[ret].active = true;
            return ret;
        }
        ++ret;
    }
}

#ifdef ANDROID
int32_t HandleAndroidMotion(struct android_app* app, AInputEvent* ev)
{
    int32_t action = AMotionEvent_getAction(ev);
    if (action == AMOTION_EVENT_ACTION_CANCEL)
    {
        FLYLOG(LT_INFO, "Cancelled Touch Motion Event...");
        // Cancelled Event
        return -1;
    }
    FLYLOG(LT_INFO, "Reading Touch Motion Event...");
    int num_pointers = AMotionEvent_getPointerCount(ev);
    if (num_pointers >= MAX_POINTERS) return -1;
    int whichsource = action >> 8;
    action &= AMOTION_EVENT_ACTION_MASK;
    bool motion_ended = true;
    for (int i = 0; i < num_pointers; ++i)
    {
        FLYLOG(LT_INFO, "Get Pointer %d Status...", i);
        int x, y;
        int pointer = GetPointer(AMotionEvent_getPointerId(ev, i));
        x = AMotionEvent_getX(ev, i);
        y = AMotionEvent_getY(ev, i);
        FLY_Pointer& p = fly_pointers[pointer];
        FLY_Gesture& g = p.gesture;
        if(whichsource != p.id) continue;
        switch (action)
        {
        case AMOTION_EVENT_ACTION_DOWN:
            p.active = true;
            g.start_x = x;
            g.start_y = y;
            ANativeActivity_showSoftInput( app->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED );
            FLYLOG(LT_INFO, "Pointer %d: Action Down - %d %d...", i, x, y);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            g.refresh_bucket += g.timer.ReadMilliSec();
            if(g.refresh_bucket >= GESTURE_REFRESH)
            {
                g.refresh_bucket -= GESTURE_REFRESH;
                // Add point to be tracked;

                FLYLOG(LT_INFO, "Pointer %d: Action Move - %d %d...", i, x, y);
            }
            break;
        case AMOTION_EVENT_ACTION_UP:
            p.active = false;
            g.end_x = x;
            g.end_y = y;
            FLYLOG(LT_INFO, "Pointer %d: Action Up - %d %d...", i, x, y);
            break;
        }
        FLYLOG(LT_INFO, "Test if pointer is Active");
        if(motion_ended == true) motion_ended = !p.active;
    }
    
    if(motion_ended) 
    {
        FLYLOG(LT_INFO, "Evaluate Motion event...");
        //FLY_EvalMotionEvent();
    }

    return 1;
}

#elif defined _WIN32 || defined __linux__
#   define USE_GLFW
#   include <GLFW/glfw3.h>
#   include <vector>
    extern std::vector<GLFWwindow*> glfw_windows;
    extern std::vector<FLY_Window*> fly_windows;
    static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        keyboard[key].prev_state = keyboard[key].state;
        keyboard[key].state = action;
        keyboard[key].key_callback(keyboard[key].prev_state, keyboard[key].state);
        //printf("Keyboard Key %i: %i\n", key, action);
    }

    static void GLFW_MouseEnterLeaveCallback(GLFWwindow* window, int entered)
    {
        for(int i = 0; i < glfw_windows.size(); ++i)
            if (glfw_windows[i] == window)
            {
                fly_windows[i]->mouse_in = entered;
                break;
            }
    }

    static void GLFW_MousePosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        mouse.prev_delta_x = mouse.x - mouse.prev_x;
        mouse.prev_delta_y = mouse.y - mouse.prev_y;
        mouse.prev_x = mouse.x;
        mouse.prev_y = mouse.y;
        mouse.x = xpos;
        mouse.y = ypos;
    }
    static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        mouse.mbuttons[button].prev_state = mouse.mbuttons[button].state;
        mouse.mbuttons[button].state = action;
        mouse.mbuttons[button].key_callback(mouse.mbuttons[button].prev_state, mouse.mbuttons[button].state);
    }

    static void GLFW_MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        mouse.prev_scrollx = mouse.scrollx;
        mouse.prev_scrolly = mouse.scrolly;
        mouse.scrollx = xoffset;
        mouse.scrolly = yoffset;
    }

#endif



    
void FLYINPUT_Init(uint16 window)
{
#ifdef ANDROID
#elif defined USE_GLFW
    glfwSetKeyCallback(glfw_windows[window], GLFW_KeyCallback);
    glfwSetCursorEnterCallback(glfw_windows[window], GLFW_MouseEnterLeaveCallback);
    glfwSetCursorPosCallback(glfw_windows[window], GLFW_MousePosCallback);
    glfwSetMouseButtonCallback(glfw_windows[window], GLFW_MouseButtonCallback);
    glfwSetScrollCallback(glfw_windows[window], GLFW_MouseScrollCallback);
#endif
}



void FLYINPUT_Process(uint16 window)
{
#ifdef ANDROID
    int events;
    struct android_poll_source* source;
    while( ALooper_pollAll(0,0, &events, (void**)&source) >= 0)
    {
        if(source!=NULL)
        {
            source->process(app, source);
        }
    }
#elif defined USE_GLFW
    glfwPollEvents();
#endif
}
