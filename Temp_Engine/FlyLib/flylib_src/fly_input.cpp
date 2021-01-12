#ifdef INPUT_SOLO
#   include "fly_input.h"
#else
#   include "fly_lib.h"
#endif


#ifdef ANDROID

// Strong Case to Make non input/Key things, part of core
#include <android_native_app_glue.h>



// This should not be core
int32_t HandleAndroidKey(struct android_app* app, AInputEvent* ev);
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
            ret = HandleAndroidKey(app, ev);
            break;
        case AINPUT_EVENT_TYPE_MOTION:
            ret = HandleAndroidMotion(app, ev);
            break;
    }


    //int32 action = AInputEvent_getAction(ev);
    return 1;
}

#endif
void EmptyKey(int32 code, int32 state) { FLYPRINT(LT_INFO, "Key %c: %d", code, state); }
KeyCallback key_fun = EmptyKey;
KeyCallback FLYINPUT_SetKeyCallback(KeyCallback fly_key_fn) 
{ 
    KeyCallback ret = key_fun;
    key_fun = fly_key_fn; 
    return ret;
}

FLY_Mouse mouse;
void EmptyMouseFloatCallback(float x, float y) { FLYPRINT(LT_INFO, "Mouse %.2f,%.2f", x, y); }
MouseFloatCallback FLYINPUT_SetMousePosCallback(MouseFloatCallback position)
{
    MouseFloatCallback ret = mouse.pos_callback;
    mouse.pos_callback = position;
    return ret;
}
MouseFloatCallback FLYINPUT_SetMouseScrollCallback(MouseFloatCallback scroll)
{
    MouseFloatCallback ret = mouse.scroll_callback;
    mouse.scroll_callback = scroll;
    return ret;
}
FLY_Key keyboard[MAX_KEYS];
#include <list>
std::list<int> char_buffer;

int FLYINPUT_GetCharFromBuffer()
{
    int ret = (char_buffer.size() > 0) ? char_buffer.front() : -1;
    if(ret != -1) char_buffer.pop_front();
    return ret;
}

FLYINPUT_Actions FLYINPUT_GetMouseButton(int button)
{
    if(button >= MAX_MOUSE_BUTTONS) return FLY_ACTION_UNKNOWN;
    return (FLYINPUT_Actions)mouse.mbuttons[button].state;
}

void FLYINPUT_GetMousePos(float* x, float* y)
{
    *x = mouse.x;
    *y = mouse.y;
}

void FLYINPUT_GetMouseWheel(float* v, float* h)
{
    if (v != NULL) *v = mouse.scrolly;
    if (h != NULL) *h = mouse.scrollx;
}

void FLYINPUT_UpdateMouseFromPointer(float xpos, float ypos, int state, int num_pointers)
{
    mouse.prev_x = mouse.x;
    mouse.prev_y = mouse.y;
    mouse.x = xpos;
    mouse.y = ypos;
    mouse.pos_callback(mouse.x, mouse.y);

    mouse.mbuttons[0].prev_state = mouse.mbuttons[0].state;
    mouse.mbuttons[0].state =  state;
    mouse.mbuttons[0].callback(0, mouse.mbuttons[0].state);
}

FLY_Pointer fly_pointers[MAX_POINTERS];

static void ResetPointers()
{
    for (int i = 0; i < MAX_POINTERS; ++i)
    {
        fly_pointers[i].active = false;
        fly_pointers[i].id = INT32_MAX;
        FLY_Gesture& g = fly_pointers[i].gesture;
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

static int GetPointer(int32_t id)
{
    int ret = 0;

    while (ret < 10)
    {
        if (!fly_pointers[ret].active || fly_pointers[ret].id == id)
        {
            if(!fly_pointers[ret].active)
            {
                fly_pointers[ret].id = id;
                fly_pointers[ret].active = true;
                fly_pointers[ret].gesture.timer.Start();
            }
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
        FLYPRINT(LT_INFO, "Cancelled Touch Motion Event...");
        // Cancelled Event
        return -1;
    }
    FLYPRINT(LT_INFO, "Reading Touch Motion Event...");
    int num_pointers = AMotionEvent_getPointerCount(ev);
    if (num_pointers >= MAX_POINTERS) return -1;
    int whichsource = action >> 8;
    APPLY_MASK(action, AMOTION_EVENT_ACTION_MASK);
    bool motion_ended = true;
    // On Pointer 0, update mouse data for other libraries to use
    for (int i = 0; i < num_pointers; ++i)
    {
        FLYPRINT(LT_INFO, "Get Pointer %d Status...", i);
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
            g.start_x = x;
            g.start_y = y;
            p.timer.Start();
            ANativeActivity_showSoftInput( app->activity, ANATIVEACTIVITY_SHOW_SOFT_INPUT_FORCED );
            if(i == 0) FLYINPUT_UpdateMouseFromPointer(x, y, FLYINPUT_Actions::FLY_ACTION_PRESS, num_pointers);
            FLYPRINT(LT_INFO, "Pointer %d: Action Down - %d %d...", i, x, y);
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
                if(i == 0) FLYINPUT_UpdateMouseFromPointer(x, y, FLYINPUT_Actions::FLY_ACTION_REPEAT, num_pointers);
                FLYPRINT(LT_INFO, "Pointer %d: Action Move - %d %d...", i, x, y);
            }
            break;
        case AMOTION_EVENT_ACTION_UP:
            p.active = false;
            g.end_x = x;
            g.end_y = y;
            g.timer.Stop();
            p.timer.Kill();
            FLYPRINT(LT_INFO, "Pointer %d: Action Up - %d %d...", i, x, y);
            if(i == 0) FLYINPUT_UpdateMouseFromPointer(x, y, FLYINPUT_Actions::FLY_ACTION_RELEASE, num_pointers);
            break;
        }
        if(motion_ended == true) motion_ended = !p.active;
    }
    
    if(motion_ended) 
    {
        FLYPRINT(LT_INFO, "Evaluate Motion event...");
        FLYINPUT_EvalGesture();
    }

    return 1;
}

#include <jni.h>
#include <android/native_activity.h>
// From https://github.com/cntools/rawdraw/blob/master/CNFGEGLDriver.c l(616)
int AndroidGetUnicodeChar(int key_code, int meta_state)
{
    int event_type = AKEY_EVENT_ACTION_DOWN;
    // Get Java Environment
    JNIEnv * env = 0;
	JNIEnv ** env_ptr = &env;
	JavaVM ** jnii_ptr = &my_app->activity->vm;
	JavaVM * jnii = my_app->activity->vm;
    
    jnii->AttachCurrentThread( env_ptr, NULL);
	env = (*env_ptr);

	jclass class_key_event = env->FindClass("android/view/KeyEvent");
	int unicodeKey;

	jmethodID method_get_unicode_char = env->GetMethodID(class_key_event, "getUnicodeChar", "(I)I");
	jmethodID event_constructor = env->GetMethodID(class_key_event, "<init>", "(II)V");
	jobject event_obj = env->NewObject(class_key_event, event_constructor, event_type, key_code);

	unicodeKey = env->CallIntMethod(event_obj, method_get_unicode_char, meta_state );

	// Finished with the JVM.
	jnii->DetachCurrentThread();

	return unicodeKey;
}

int32_t HandleAndroidKey(struct android_app* app, AInputEvent* ev)
{
    int code = AKeyEvent_getKeyCode(ev);

    int unicode = AndroidGetUnicodeChar( code, AMotionEvent_getMetaState(ev));
    if(AKeyEvent_getAction(ev) == 1) char_buffer.push_back(unicode);

    if(unicode && unicode < MAX_KEYS)
    {
        keyboard[code].prev_state = keyboard[code].state;
        keyboard[code].state = AKeyEvent_getAction(ev);
        keyboard[code].key_callback(unicode, keyboard[code].prev_state, keyboard[code].state);
        key_fun(keyboard[code]);
    }
    else
    {
        FLY_ConsolePrint(LT_WARNING, "Unicode Value not supported...");
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
        keyboard[key].callback(key, keyboard[key].state);
        key_fun(key, keyboard[key].state);
    }

    static void GLFW_CharCallback(GLFWwindow* window, uint32 codepoint)
    {
        char_buffer.push_back(codepoint);
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

#endif


void FLYINPUT_SetMousePos(float x, float y)
{
#ifdef USE_GLFW
    //glfwSetCursor(glfw_windows[0], );
#endif
}
    
bool FLYINPUT_Init(uint16 window)
{
    bool ret = true;
#ifdef ANDROID
#elif defined USE_GLFW
    glfwSetInputMode(glfw_windows[window], GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetKeyCallback(glfw_windows[window], GLFW_KeyCallback);
    glfwSetCursorEnterCallback(glfw_windows[window], GLFW_MouseEnterLeaveCallback);
    glfwSetCursorPosCallback(glfw_windows[window], GLFW_MousePosCallback);
    glfwSetMouseButtonCallback(glfw_windows[window], GLFW_MouseButtonCallback);
    glfwSetScrollCallback(glfw_windows[window], GLFW_MouseScrollCallback);
    glfwSetCharCallback(glfw_windows[window], GLFW_CharCallback);
#endif

    return ret;
}

bool FLYINPUT_Close()
{
    bool ret = true;
    
    return ret;
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
            source->process(my_app, source);
        }
    }
#elif defined USE_GLFW
    glfwPollEvents();
#endif
}


void FLYINPUT_DisplaySoftwareKeyboard(bool show)
{
#ifdef ANDROID
    jint lflags = 0;
    
    // Get Java Environment
    JNIEnv * env = 0;
	JNIEnv ** env_ptr = &env;
	JavaVM ** jnii_ptr = &my_app->activity->vm;
	JavaVM * jnii = *jnii_ptr;
    
    jnii->AttachCurrentThread( env_ptr, NULL);
	env = (*env_ptr);
    jclass activity_class = env->FindClass("android/app/NativeActivity");
    jobject lnative_activity = my_app->activity->clazz;

    // Retrieves Context.INPUT_METHOD_SERVICE.
    jclass class_context = env->FindClass("android/content/Context");
	jfieldID field_INPUT_METHOD_SERVICE = env->GetStaticFieldID(class_context, "INPUT_METHOD_SERVICE", "Ljava/lang/String;" );
	jobject INPUT_METHOD_SERVICE = env->GetStaticObjectField(class_context, field_INPUT_METHOD_SERVICE );

    // Runs getSystemService(Context.INPUT_METHOD_SERVICE).
	jclass class_input_method_manager = env->FindClass("android/view/inputmethod/InputMethodManager" );
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
	} else {
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

FLYINPUT_Actions FLYINPUT_DetectGesture(FLY_Pointer& p)
{
    // Evaluate what the pointer has done during the time it was tracked
    int16 delta_x = p.gesture.end_x - p.gesture.start_x;
    int16 delta_y = p.gesture.end_y - p.gesture.start_y;
    uint16 screen_w, screen_h;
    FLYDISPLAY_GetWindowSize(0, &screen_w, &screen_h);

    uint16 abs_delta_x = delta_x;
    uint16 abs_delta_y = delta_y;
    float dist_perc_x = 100*abs_delta_x / (float)screen_w;
    float dist_perc_y = 100*abs_delta_y / (float)screen_h;

    uint16 time = p.gesture.timer.ReadMilliSec();
    // Tap Gesture
    if(time <= GESTURE_REFRESH || (abs_delta_x < 5 && abs_delta_y < 5))
    {
        return FLYINPUT_Actions::FLY_ACTION_TAP;
    }
    // Swipe Gesture
    else if(time > GESTURE_REFRESH && time <= 3*GESTURE_REFRESH)
    {
        if(abs_delta_x > abs_delta_y)
            return (FLYINPUT_Actions)(FLY_ACTION_SWIPE_LEFT+(int)(delta_x > 0)); // Swipe Horizontal
        else
            return (FLYINPUT_Actions)(FLY_ACTION_SWIPE_UP+(int)(delta_y < 0));  // Swipe Vertical
    }

    return FLYINPUT_Actions::FLY_ACTION_UNKNOWN;
}

FLYINPUT_Actions FLYINPUT_EvalGesture()
{
    int num_pointers = 0;
    FLYINPUT_Actions actions[MAX_POINTERS];
    for(int i = 0; i < MAX_POINTERS; ++i) 
    {
        if(fly_pointers[i].id == INT32_MAX || !fly_pointers[i].gesture.timer.IsActive()) 
                    break;
        actions[i] = FLYINPUT_DetectGesture(fly_pointers[i]);
        FLYPRINT(LT_INFO, "Pointer %d: FLYINPUT_ACTION::%d", i, actions[i]);
    }

    ResetPointers();

    return FLYINPUT_Actions::FLY_ACTION_UNKNOWN;
}