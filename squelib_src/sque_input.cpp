#ifdef INPUT_SOLO
#   include "sque_input.h"
#else
#   include "squelib.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DEFINITION ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugKey(int32_t code, int32_t state) { SQUE_PRINT(LT_INFO, "Key %c: %d", code, state); }
KeyCallback key_fun = DebugKey;

SQUE_Key keyboard[MAX_KEYS];
SQUE_Mouse mouse;
void DebugMouseFloatCallback(float x, float y) { SQUE_PRINT(LT_INFO, "Mouse %.2f,%.2f", x, y); }

#include <list> // i don't like including libraries, but will do until I have a proven easier alternative
std::list<int> char_buffer;

SQUE_Pointer sque_pointers[MAX_POINTERS];
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLATFORM SPECIFICS ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


#if defined(ANDROID)

#include <android_native_app_glue.h>
#include <jni.h>
#include <android/native_activity.h>
extern int graphics_backend_started;
extern struct android_app* my_app;

// From https://github.com/cntools/rawdraw/blob/master/CNFGEGLDriver.c l(616)
int AndroidGetUnicodeChar(int key_code, int meta_state)
{
    int event_type = AKEY_EVENT_ACTION_DOWN;
    // Get Java Environment
    JNIEnv* env = 0;
    JNIEnv** env_ptr = &env;
    JavaVM** jnii_ptr = &my_app->activity->vm;
    JavaVM* jnii = my_app->activity->vm;

    jnii->AttachCurrentThread(env_ptr, NULL);
    env = (*env_ptr);

    jclass class_key_event = env->FindClass("android/view/KeyEvent");
    int unicodeKey;

    jmethodID method_get_unicode_char = env->GetMethodID(class_key_event, "getUnicodeChar", "(I)I");
    jmethodID event_constructor = env->GetMethodID(class_key_event, "<init>", "(II)V");
    jobject event_obj = env->NewObject(class_key_event, event_constructor, event_type, key_code);

    unicodeKey = env->CallIntMethod(event_obj, method_get_unicode_char, meta_state);

    // Finished with the JVM.
    jnii->DetachCurrentThread();

    return unicodeKey;
}

int32_t HandleAndroidKey(struct android_app* app, AInputEvent* ev)
{
    int code = AKeyEvent_getKeyCode(ev);

    int unicode = AndroidGetUnicodeChar(code, AMotionEvent_getMetaState(ev));
    if (AKeyEvent_getAction(ev) == 1) char_buffer.push_back(unicode);

    if (unicode && unicode < MAX_KEYS)
    {
        keyboard[code].prev_state = keyboard[code].state;
        keyboard[code].state = AKeyEvent_getAction(ev);
        keyboard[code].callback(unicode, keyboard[code].state);
        key_fun(code, keyboard[code].state);
    }
    else
    {
        SQUE_ConsolePrint(LT_WARNING, "Unicode Value not supported...");
    }

    return 1;
}

int32_t HandleAndroidMotion(struct android_app* app, AInputEvent* ev);
int32_t HandleAndroidInput(struct android_app* app, AInputEvent* ev)
{
    int32_t evtype = AInputEvent_getType(ev);
    int32_t ret = 1;
    switch(evtype)
    {
        case AINPUT_EVENT_TYPE_FOCUS:
            break;
        case AINPUT_EVENT_TYPE_KEY:
            ret = HandleAndroidKey(app, ev);
            break;
        case AINPUT_EVENT_TYPE_MOTION:
            ret = HandleAndroidMotion(app, ev);
            break;
    }
    return 1;
}

#endif

#if defined(USE_GLFW)

#include <GLFW/glfw3.h>
extern std::vector<GLFWwindow*> glfw_windows;

static void GLFW_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    keyboard[key].prev_state = keyboard[key].state;
    keyboard[key].state = action;
    keyboard[key].callback(key, keyboard[key].state);
    key_fun(key, keyboard[key].state);
}

static void GLFW_CharCallback(GLFWwindow* window, uint32_t codepoint)
{
    char_buffer.push_back(codepoint);
}

static void GLFW_MousePosCallback(GLFWwindow* window, double xpos, double ypos)
{
    mouse.prev_x = mouse.x;
    mouse.prev_y = mouse.y;
    mouse.x = (float)xpos;
    mouse.y = (float)ypos;
    mouse.pos_callback(mouse.x, mouse.y);
}
static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    mouse.mbuttons[button].prev_state = mouse.mbuttons[button].state;
    mouse.mbuttons[button].state = action;
    mouse.mbuttons[button].callback(button, mouse.mbuttons[button].state);
}

static void GLFW_MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    mouse.prev_scrollx = mouse.scrollx;
    mouse.prev_scrolly = mouse.scrolly;
    mouse.scrollx = xoffset;
    mouse.scrolly = yoffset;
    mouse.scroll_callback(mouse.scrollx, mouse.scrolly);
}

#else
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE SETTERS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_INPUT_UpdateMouseFromPointer(float xpos, float ypos, int state, int num_pointers)
{
    mouse.prev_x = mouse.x;
    mouse.prev_y = mouse.y;
    mouse.x = xpos;
    mouse.y = ypos;
    mouse.pos_callback(mouse.x, mouse.y);

    mouse.mbuttons[0].prev_state = mouse.mbuttons[0].state;
    mouse.mbuttons[0].state = state;
    mouse.mbuttons[0].callback(0, mouse.mbuttons[0].state);
}



static void ResetPointers()
{
    for (int i = 0; i < MAX_POINTERS; ++i)
    {
        sque_pointers[i].active = false;
        sque_pointers[i].id = INT32_MAX;
        SQUE_Gesture& g = sque_pointers[i].gesture;
        g.timer.Kill();
        g.start_x = INT32_MAX;
        g.start_y = INT32_MAX;
        g.end_x = INT32_MAX;
        g.end_y = INT32_MAX;
        g.refresh_bucket = 0;
        for (int j = 0; j < MAX_MIDPOINTS; ++j)
        {
            g.midpoints[j][0] = 0;
            g.midpoints[j][1] = 0;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE GETTERS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


static int GetPointer(int32_t id)
{
    int ret = 0;

    while (ret < 10)
    {
        if (!sque_pointers[ret].active || sque_pointers[ret].id == id)
        {
            if (!sque_pointers[ret].active)
            {
                sque_pointers[ret].id = id;
                sque_pointers[ret].active = true;
                sque_pointers[ret].gesture.timer.Start();
            }
            return ret;
        }
        ++ret;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZING & STATE MANAGEMENT ///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_INPUT_Init()
{
    SQUE_INPUT_InitForWindow(0);
}
void SQUE_INPUT_InitForWindow(uint16_t window)
{

#if defined USE_GLFW
    if (window > glfw_windows.size())
    {
        SQUE_PRINT(LT_WARNING, "Tried to Init input for window out of range!");
        return;
    }

    glfwSetInputMode(glfw_windows[window], GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetKeyCallback(glfw_windows[window], GLFW_KeyCallback);
    
    glfwSetCursorPosCallback(glfw_windows[window], GLFW_MousePosCallback);
    glfwSetMouseButtonCallback(glfw_windows[window], GLFW_MouseButtonCallback);
    glfwSetScrollCallback(glfw_windows[window], GLFW_MouseScrollCallback);
    glfwSetCharCallback(glfw_windows[window], GLFW_CharCallback);
#endif
}

bool SQUE_INPUT_Close()
{
    bool ret = true;

    return ret;
}


void SQUE_INPUT_Process(uint16_t window)
{
#ifdef ANDROID
    int events;
    struct android_poll_source* source;
    while (ALooper_pollAll(0, 0, &events, (void**)&source) >= 0)
    {
        if (source != NULL)
        {
            source->process(my_app, source);
        }
    }
#elif defined USE_GLFW
    glfwPollEvents();
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE / UTILITIES /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_INPUT_DisplaySoftwareKeyboard(bool show)
{
#ifdef ANDROID
    jint lflags = 0;

    // Get Java Environment
    JNIEnv* env = 0;
    JNIEnv** env_ptr = &env;
    JavaVM** jnii_ptr = &my_app->activity->vm;
    JavaVM* jnii = *jnii_ptr;

    jnii->AttachCurrentThread(env_ptr, NULL);
    env = (*env_ptr);
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jobject lnative_activity = my_app->activity->clazz;

    // Retrieves Context.INPUT_METHOD_SERVICE.
    jclass class_context = env->FindClass("android/content/Context");
    jfieldID field_INPUT_METHOD_SERVICE = env->GetStaticFieldID(class_context, "INPUT_METHOD_SERVICE", "Ljava/lang/String;");
    jobject INPUT_METHOD_SERVICE = env->GetStaticObjectField(class_context, field_INPUT_METHOD_SERVICE);

    // Runs getSystemService(Context.INPUT_METHOD_SERVICE).
    jclass class_input_method_manager = env->FindClass("android/view/inputmethod/InputMethodManager");
    jmethodID method_get_system_service = env->GetMethodID(activity_class, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
    jobject linput_method_manager = env->CallObjectMethod(lnative_activity, method_get_system_service, INPUT_METHOD_SERVICE);

    // Runs getWindow().getDecorView().
    jmethodID method_get_window = env->GetMethodID(activity_class, "getWindow", "()Landroid/view/Window;");
    jobject lwindow = env->CallObjectMethod(lnative_activity, method_get_window);
    jclass class_window = env->FindClass("android/view/Window");
    jmethodID method_get_decor_view = env->GetMethodID(class_window, "getDecorView", "()Landroid/view/View;");
    jobject ldecor_view = env->CallObjectMethod(lwindow, method_get_decor_view);

    if (show) {
        // Runs lInputMethodManager.showSoftInput(...).
        jmethodID method_show_soft_input = env->GetMethodID(class_input_method_manager, "showSoftInput", "(Landroid/view/View;I)Z");
        /*jboolean lResult = */env->CallBooleanMethod(linput_method_manager, method_show_soft_input, ldecor_view, lflags);
    }
    else {
        // Runs lwindow.getViewToken()
        jclass class_view = env->FindClass("android/view/View");
        jmethodID method_get_window_token = env->GetMethodID(class_view, "getWindowToken", "()Landroid/os/IBinder;");
        jobject lbinder = env->CallObjectMethod(ldecor_view, method_get_window_token);

        // lInputMethodManager.hideSoftInput(...).
        jmethodID method_hide_soft_input = env->GetMethodID(class_input_method_manager, "hideSoftInputFromWindow", "(Landroid/os/IBinder;I)Z");
        /*jboolean lRes = */env->CallBooleanMethod(linput_method_manager, method_hide_soft_input, lbinder, lflags);
    }

    // Finished with the JVM.
    jnii->DetachCurrentThread();

#endif
}

SQUE_INPUT_Actions SQUE_INPUT_DetectGesture(SQUE_Pointer& p)
{
    // Evaluate what the pointer has done during the time it was tracked
    int16_t delta_x = p.gesture.end_x - p.gesture.start_x;
    int16_t delta_y = p.gesture.end_y - p.gesture.start_y;
    int32_t screen_w, screen_h;
    SQUE_DISPLAY_GetWindowSize(0, &screen_w, &screen_h);

    uint16_t abs_delta_x = delta_x;
    uint16_t abs_delta_y = delta_y;
    float dist_perc_x = 100 * abs_delta_x / (float)screen_w;
    float dist_perc_y = 100 * abs_delta_y / (float)screen_h;

    uint16_t time = p.gesture.timer.ReadMilliSec();
    // Tap Gesture
    if (time <= GESTURE_REFRESH || (abs_delta_x < 5 && abs_delta_y < 5))
    {
        return SQUE_INPUT_Actions::SQUE_ACTION_TAP;
    }
    // Swipe Gesture
    else if (time > GESTURE_REFRESH && time <= 3 * GESTURE_REFRESH)
    {
        if (abs_delta_x > abs_delta_y)
            return (SQUE_INPUT_Actions)(SQUE_ACTION_SWIPE_LEFT + (int)(delta_x > 0)); // Swipe Horizontal
        else
            return (SQUE_INPUT_Actions)(SQUE_ACTION_SWIPE_UP + (int)(delta_y < 0));  // Swipe Vertical
    }
    else if (time > GESTURE_REFRESH && time > 3 * GESTURE_REFRESH)
    {

    }

    return SQUE_INPUT_Actions::SQUE_ACTION_UNKNOWN;
}

SQUE_INPUT_Actions SQUE_INPUT_EvalGesture()
{
    int num_pointers = 0;
    SQUE_INPUT_Actions actions[MAX_POINTERS];
    for (int i = 0; i < MAX_POINTERS; ++i)
    {
        if (sque_pointers[i].id == INT32_MAX || !sque_pointers[i].gesture.timer.IsActive())
            break;
        actions[i] = SQUE_INPUT_DetectGesture(sque_pointers[i]);
        SQUE_PRINT(LT_INFO, "Pointer %d: SQUE_INPUT_ACTION::%d", i, actions[i]);
    }

    ResetPointers();

    return SQUE_INPUT_Actions::SQUE_ACTION_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SETTERS ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_INPUT_SetMousePos(float x, float y)
{
#ifdef USE_GLFW
    //glfwSetCursor(glfw_windows[0], );
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GETTERS ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int SQUE_INPUT_GetCharFromBuffer()
{
    int ret = (char_buffer.size() > 0) ? char_buffer.front() : -1;
    if (ret != -1) char_buffer.pop_front();
    return ret;
}

SQUE_INPUT_Actions SQUE_INPUT_GetMouseButton(int button)
{
    if (button >= MAX_MOUSE_BUTTONS) return SQUE_ACTION_UNKNOWN;
    return (SQUE_INPUT_Actions)mouse.mbuttons[button].state;
}

void SQUE_INPUT_GetMousePos(float* x, float* y)
{
    *x = mouse.x;
    *y = mouse.y;
}

void SQUE_INPUT_GetMouseWheel(float* v, float* h)
{
    if (v != NULL) *v = mouse.scrolly;
    if (h != NULL) *h = mouse.scrollx;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACK SETTERS //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


KeyCallback SQUE_INPUT_SetKeyCallback(KeyCallback sque_key_fn) 
{ 
    KeyCallback ret = key_fun;
    key_fun = sque_key_fn; 
    return ret;
}


MouseFloatCallback SQUE_INPUT_SetMousePosCallback(MouseFloatCallback position)
{
    MouseFloatCallback ret = mouse.pos_callback;
    mouse.pos_callback = position;
    return ret;
}
MouseFloatCallback SQUE_INPUT_SetMouseScrollCallback(MouseFloatCallback scroll)
{
    MouseFloatCallback ret = mouse.scroll_callback;
    mouse.scroll_callback = scroll;
    return ret;
}





#ifdef ANDROID
int32_t HandleAndroidMotion(struct android_app* app, AInputEvent* ev)
{
    int32_t action = AMotionEvent_getAction(ev);
    if (action == AMOTION_EVENT_ACTION_CANCEL)
    {
        SQUE_PRINT(LT_INFO, "Cancelled Touch Motion Event...");
        // Cancelled Event
        return -1;
    }
    SQUE_PRINT(LT_INFO, "Reading Touch Motion Event...");
    int num_pointers = AMotionEvent_getPointerCount(ev);
    if (num_pointers >= MAX_POINTERS) return -1;
    int whichsource = action >> 8;
    APPLY_MASK(action, AMOTION_EVENT_ACTION_MASK);
    bool motion_ended = true;
    // On Pointer 0, update mouse data for other libraries to use
    for (int i = 0; i < num_pointers; ++i)
    {
        SQUE_PRINT(LT_INFO, "Get Pointer %d Status...", i);
        int x, y;
        int pointer = GetPointer(AMotionEvent_getPointerId(ev, i));
        x = AMotionEvent_getX(ev, i);
        y = AMotionEvent_getY(ev, i);
        SQUE_Pointer& p = sque_pointers[pointer];
        SQUE_Gesture& g = p.gesture;
        if(whichsource != p.id) continue;
        switch (action)
        {
        case AMOTION_EVENT_ACTION_DOWN:
            g.start_x = x;
            g.start_y = y;
            p.timer.Start();
            ANativeActivity_showSoftInput( app->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED );
            if(i == 0) SQUE_INPUT_UpdateMouseFromPointer(x, y, SQUE_INPUT_Actions::SQUE_ACTION_PRESS, num_pointers); // THIS IS A HORRIBLE HACK, on the long run I can't add proper pointer interaction
            SQUE_PRINT(LT_INFO, "Pointer %d: Action Down - %d %d...", i, x, y);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            g.refresh_bucket += p.timer.ReadMilliSec();
            if(g.refresh_bucket >= GESTURE_REFRESH)
            {
                p.timer.Start();
                g.refresh_bucket -= GESTURE_REFRESH;
                // Add point to be tracked;
                if(g.start_x == INT32_MAX)
                {
                    g.start_x = x;
                    g.start_y = y;
                }
                else
                {
                    g.end_x = x;
                    g.end_y = y;
                }
                if(i == 0) SQUE_INPUT_UpdateMouseFromPointer(x, y, SQUE_INPUT_Actions::SQUE_ACTION_REPEAT, num_pointers); // THIS IS A HORRIBLE HACK, on the long run I can't add proper pointer interaction
                SQUE_PRINT(LT_INFO, "Pointer %d: Action Move - %d %d...", i, x, y);
            }
            break;
        case AMOTION_EVENT_ACTION_UP:
            p.active = false;
            g.end_x = x;
            g.end_y = y;
            g.timer.Stop();
            p.timer.Kill();
            SQUE_PRINT(LT_INFO, "Pointer %d: Action Up - %d %d...", i, x, y);
            if(i == 0) SQUE_INPUT_UpdateMouseFromPointer(x, y, SQUE_INPUT_Actions::SQUE_ACTION_RELEASE, num_pointers); // THIS IS A HORRIBLE HACK, on the long run I can't add proper pointer interaction
            break;
        }
        if(motion_ended == true) motion_ended = !p.active;
    }
    
    if(motion_ended) 
    {
        SQUE_PRINT(LT_INFO, "Evaluate Motion event...");
        SQUE_INPUT_EvalGesture();
    }

    return 1;
}



#elif defined _WIN32 || defined __linux__
#   define USE_GLFW

#endif

