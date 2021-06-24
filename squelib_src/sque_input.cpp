#ifdef INPUT_SOLO
#   include "sque_input.h"
#else
#   include "squelib.h"
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DEFINITION ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void DebugKey(int32_t code, int32_t state) { /*SQUE_PRINT(LT_INFO, "Key %c: %d", code, state);*/ }
KeyCallback* key_fun = DebugKey;

SQUE_Key keyboard[MAX_KEYS];
void DebugMouseFloatCallback(float x, float y) { /*SQUE_PRINT(LT_INFO, "Pointer %.2f,%.2f", x, y);*/ }

std::list<int> char_buffer;

float scrollx = INT32_MAX, scrolly = INT32_MAX;
MouseFloatCallback* scroll_callback = DebugMouseFloatCallback;

SQUE_Key mouse_buttons[MAX_MOUSE_BUTTONS];
SQUE_Pointer pointers[MAX_POINTERS];
SQUE_Gesture gestures[MAX_POINTERS];
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLATFORM SPECIFICS ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


#if defined(ANDROID)

#include <android_native_app_glue.h>
#include <jni.h>
#include <android/native_activity.h>
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
extern sque_vec<GLFWwindow*> glfw_windows;

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
    pointers[0].x = (float)xpos;
    pointers[0].y = (float)ypos;
    pointers[0].pos_callback(pointers[0].x, pointers[0].y);
}
static void GLFW_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    mouse_buttons[button].prev_state = mouse_buttons[button].state;
    mouse_buttons[button].state = action;
    mouse_buttons[button].callback(button, mouse_buttons[button].state);
    pointers[0].active = mouse_buttons[button].state > SQUE_ACTION_RELEASE;
}

static void GLFW_MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    scrollx = xoffset;
    scrolly = yoffset;
    scroll_callback(scrollx, scrolly);
}

#else
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE SETTERS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ResetPointers()
{
    for (int i = 0; i < MAX_POINTERS; ++i)
    {
        pointers[i].active = false;
        pointers[i].id = INT32_MAX;
        SQUE_Gesture& g = gestures[i];
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
        if (!pointers[ret].active || pointers[ret].id == id)
        {
            if (!pointers[ret].active)
            {
                pointers[ret].id = id;
                pointers[ret].active = true;
                gestures[ret].timer.Start();
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
    //pointers[0].active = true;
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

SQUE_INPUT_Actions SQUE_INPUT_DetectGesture(const SQUE_Gesture& g)
{
    // Evaluate what the pointer has done during the time it was tracked
    int16_t delta_x = g.end_x - g.start_x;
    int16_t delta_y = g.end_y - g.start_y;
    int32_t screen_w, screen_h;
    SQUE_DISPLAY_GetWindowSize(&screen_w, &screen_h);

    uint16_t abs_delta_x = delta_x;
    uint16_t abs_delta_y = delta_y;
    float dist_perc_x = 100 * abs_delta_x / (float)screen_w;
    float dist_perc_y = 100 * abs_delta_y / (float)screen_h;

    uint16_t time = g.timer.ReadMilliSec();
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
        if (pointers[i].id == INT32_MAX || !gestures[i].timer.IsActive())
            break;
        actions[i] = SQUE_INPUT_DetectGesture(gestures[i]);
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

void SQUE_INPUT_SetPointerActive(uint16_t pointer, bool active)
{
    pointers[pointer].active = active;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// GETTERS ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_INPUT_Actions SQUE_INPUT_GetKey(const SQUE_KeyboardKeys key)
{
    return (SQUE_INPUT_Actions)keyboard[key].state;
}

int SQUE_INPUT_GetCharFromBuffer()
{
    int ret = (char_buffer.size() > 0) ? char_buffer.front() : -1;
    if (ret != -1) char_buffer.pop_front();
    return ret;
}

SQUE_INPUT_Actions SQUE_INPUT_GetMouseButton(uint32_t button)
{
    if (button >= MAX_MOUSE_BUTTONS) return SQUE_ACTION_UNKNOWN;
    return (SQUE_INPUT_Actions)mouse_buttons[button].state;
}

void SQUE_INPUT_GetPointerAvgPos(float* x, float* y, uint16_t points)
{
    int lx = *x, ly = *y;
    *x = *y = 0;
    uint16_t inactive_p = 0;
    for (uint16_t i = 0; i < points; ++i)
    {
        inactive_p += !pointers[i].active;
        *x += (pointers[i].x) * pointers[i].active;
        *y += (pointers[i].y) * pointers[i].active;
    }
    if(points == inactive_p)
    {
        *x = lx;
        *y = ly;
    }
    else{
        *x /= (float)(points - inactive_p);
        *y /= (float)(points - inactive_p);
    }
}

bool SQUE_INPUT_GetPointerPos(float* x, float* y, uint16_t pointer)
{
    if(pointer > MAX_POINTERS) return false;
    SQUE_Pointer& p = pointers[pointer];
    *x = p.x;
    *y = p.y;

    return p.active;
}

void SQUE_INPUT_GetScroll(float* v, float* h)
{
    if (v != NULL) *v = scrolly;
    if (h != NULL) *h = scrollx;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACK SETTERS //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


KeyCallback* SQUE_INPUT_SetKeyCallback(KeyCallback* sque_key_fn) 
{ 
    KeyCallback* ret = key_fun;
    key_fun = sque_key_fn; 
    return ret;
}


MouseFloatCallback* SQUE_INPUT_SetPointerPosCallback(MouseFloatCallback* position, uint16_t pointer)
{
    MouseFloatCallback* ret = pointers[pointer].pos_callback;
    pointers[pointer].pos_callback = position;
    return ret;
}
MouseFloatCallback* SQUE_INPUT_SetScrollCallback(MouseFloatCallback* scroll)
{
    MouseFloatCallback* ret = scroll_callback;
    scroll_callback = scroll;
    return ret;
}

KeyCallback* SQUE_INPUT_SetMouseButtonCallback(int button, KeyCallback* cb)
{
    KeyCallback* ret = mouse_buttons[button].callback;
    mouse_buttons[button].callback = cb;
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
    if(num_pointers > 0)
    {   
        SQUE_PRINT(LT_INFO, "Pointer 0 as fake mouse");
        int pointer = GetPointer(AMotionEvent_getPointerId(ev,0));
        int x = AMotionEvent_getX(ev, 0);
        int y = AMotionEvent_getY(ev, 0);
        SQUE_Pointer& p = pointers[0];
        if(whichsource == p.id)
        {
            mouse_buttons[0].prev_state = mouse_buttons[0].state;
            switch (action)
            {
            case AMOTION_EVENT_ACTION_DOWN:
                mouse_buttons[0].state = SQUE_ACTION_PRESS;
                break;
            case AMOTION_EVENT_ACTION_MOVE:
                mouse_buttons[0].state = SQUE_ACTION_REPEAT;
                break;
            case AMOTION_EVENT_ACTION_UP:
                mouse_buttons[0].state = SQUE_ACTION_RELEASE;
                break;
            }
        }
    }
    for (int i = 0; i < num_pointers; ++i)
    {
        SQUE_PRINT(LT_INFO, "Get Pointer %d Status...", i);
        int x, y;
        int pointer = GetPointer(AMotionEvent_getPointerId(ev, i));
        x = AMotionEvent_getX(ev, i);
        y = AMotionEvent_getY(ev, i);
        SQUE_Pointer& p = pointers[pointer];
        SQUE_Gesture& g = gestures[pointer];
        if(whichsource != p.id) continue;
        switch (action)
        {
        case AMOTION_EVENT_ACTION_DOWN:
            p.active = true;
            p.x = x;
            p.y = y;
            g.start_x = x;
            g.start_y = y;
            p.pos_callback(x,y);
            ANativeActivity_showSoftInput( app->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED );
            SQUE_PRINT(LT_INFO, "Pointer %d: Action Down - %d %d...", i, x, y);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
        // Redo MidPoints of a Gesture properly
            SQUE_PRINT(LT_INFO, "Pointer %d: Action Move - %d %d...", i, x, y);
            p.x = x;
            p.y = y;
            p.pos_callback(x,y);
            break;
        case AMOTION_EVENT_ACTION_UP:
            p.active = false;
            p.x = x;
            p.y = y;
            g.end_x = x;
            g.end_y = y;
            g.timer.Stop();
            p.pos_callback(x,y);    
            SQUE_PRINT(LT_INFO, "Pointer %d: Action Up - %d %d...", i, x, y);
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

