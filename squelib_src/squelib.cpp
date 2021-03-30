#include "squelib.h"

// really want to stop using these includes,.... will do my own simplified string and types?
#include <string>
#include <cstring>

#if defined(ANDROID) || defined(__linux__)
#   include <unistd.h>
#else if defined(_WIN32)
#   include <windows.h>
#endif

#include <pcg_basic.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DEFINITION ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

VoidFun on_resume_callback = NULL;
VoidFun on_go_background_callback = NULL;

int SQUE_VarGetSize(int type_macro)
{
    if (type_macro == SQUE_BYTE || type_macro == SQUE_UBYTE /*add 1byte types*/)
        return 1;
    else if (type_macro == SQUE_DOUBLE/*add 8 byte types*/)
        return 8;
    else if (type_macro == SQUE_FLOAT || type_macro == SQUE_UINT || type_macro == SQUE_INT/* add 4 byte types*/)
        return 4;
    else if (type_macro == SQUE_USHORT)
        return 2;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLATFORM SPECIFICS ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ANDROID

// Putting the android entry point under window module because in
// android you acquire a native_app as if it were the window to act upon.
#include <android_native_app_glue.h>

int graphics_backend_started = 0;
struct android_app* my_app;
bool have_resumed = false;
void HandleAndroidCMD(struct android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_INIT_WINDOW");
        my_app = app;
        if (!graphics_backend_started) graphics_backend_started = 1;
        else
        {
            SQUE_PRINT(SQUE_LogType::LT_INFO, "SQUE_LIB RE-INIT");
            SQUE_LIB_Init(""); // Way to remember flags and app name
            SQUE_DISPLAY_UpdateNativeWindowSize(0);
            on_resume_callback();
            have_resumed = true;
        }
        break;
    case APP_CMD_TERM_WINDOW:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_TERM_WINDOW");
        break;
    case APP_CMD_WINDOW_RESIZED:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_WINDOW_RESIZED");
        //SQUE_DISPLAY_UpdateNativeWindowSize(0);
        break;
    case APP_CMD_WINDOW_REDRAW_NEEDED:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_WINDOW_REDRAW_NEEDED");
        break;
    case APP_CMD_CONTENT_RECT_CHANGED:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_CONTENT_RECT_CHANGED");
        // Require a window resize
        SQUE_DISPLAY_UpdateNativeWindowSize(0);
        // I don't like, this should be setup as callback to own resize (if squelib init, then it changes);
        break;
    case APP_CMD_GAINED_FOCUS:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_GAINED_FOCUS");
        break;
    case APP_CMD_LOST_FOCUS:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_LOST_FOCUS");
        break;
    case APP_CMD_CONFIG_CHANGED:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_CONFIG_CHANGED");
        break;
    case APP_CMD_LOW_MEMORY:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_LOW_MEMORY");
        break;
    case APP_CMD_START:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_START");
        break;
    case APP_CMD_RESUME:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_RESUME");
        break;
    case APP_CMD_SAVE_STATE:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_SAVE_STATE");
        // Execution should stop doing intensive stuff (render, resettign dt,...)
        on_go_background_callback();
        break;
    case APP_CMD_PAUSE:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_PAUSE");
        // Register Go To Background functions and call them
        break;
    case APP_CMD_STOP:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_STOP");
        // Execution should stop doing intensive stuff (render, resettign dt,...)

        break;
    case APP_CMD_DESTROY:
        SQUE_PRINT(SQUE_LogType::LT_INFO, "APP_CMD_DESTROY");
        SQUE_DISPLAY_SetWindowClose(0); // Set to Close main window and end execution
        break;
    }
}

extern int32_t HandleAndroidInput(struct android_app* app, AInputEvent* ev);

// On Android, This main is first call that will call your defined main
#include <android/log.h>
extern int main(int argc, char** argv);
void android_main(struct android_app* app)
{
    my_app = app;

    SQUE_PRINT(SQUE_LogType::LT_INFO, "SQUE_LIB - Android Squelib Start...");

    app->onAppCmd = HandleAndroidCMD;
    app->onInputEvent = HandleAndroidInput;

    char *argv[] = {"AppMain", 0};

    SQUE_PRINT(SQUE_LogType::LT_INFO, "SQUE_LIB - Calling App Main...");
    main(2, argv);

    SQUE_PRINT(SQUE_LogType::LT_INFO, "SQUE_LIB - Finished executing App...");
}


void AndroidSendToBack(int param)
{
    JNIEnv* env = 0;
    JNIEnv** env_ptr = &env;
    JavaVM** jnii_ptr = &my_app->activity->vm;
    JavaVM* jnii = *jnii_ptr;

    jnii->AttachCurrentThread(env_ptr, NULL);
    env = (*env_ptr);
    jobject activity = my_app->activity->clazz;

    jclass class_activity = env->FindClass("adnroid/app/Activity");
    jmethodID method_move_task_to_back = env->GetMethodID(class_activity, "moveTaskToBack", "(Z)Z");
    env->CallBooleanMethod(activity, method_move_task_to_back, param);
    jnii->DetachCurrentThread();
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION AND STATE CONTROL ////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <ctime>
static pcg32_random_t random_t;
// un struct tipo sque_lib instance
// o un struct global en el que se mantienen las variables
// Para organizar o para tener multiples instancies de una libreria
void SQUE_LIB_Init(const char* app_name)
{
    pcg32_srandom_r(&random_t, time(NULL), (intptr_t)&random_t);

    // Call Init for all loaded modules and with required flags
    // Logger Lib
    SQUE_LOGGER_Init(true);
    
    // Display Lib
    SQUE_DISPLAY_SetViewportResizeCallback(SQUE_RENDER_ChangeFramebufferSize);
    SQUE_DISPLAY_SetViewportSizeCallback(SQUE_RENDER_GetFramebufferSize);
    SQUE_DISPLAY_Init();

    // Default options, 
    int32_t options[] =
    {
        SQUE_WINDOW_CONTEXT_MAJOR, SQUE_CONTEXT_MAJOR_MIN,
        SQUE_WINDOW_CONTEXT_MINOR, SQUE_CONTEXT_MINOR_MIN,
        SQUE_DEBUG_CONTEXT, 1
    };
    SQUE_DISPLAY_NextWindow_ContextHints(options, sizeof(options)/sizeof(int32_t));
    SQUE_DISPLAY_OpenWindow(app_name);

    // For testing
    SQUE_DISPLAY_MakeContextMain(0);
    SQUE_DISPLAY_SetVSYNC(1); // Swap Interval
    // Input Lib
    SQUE_INPUT_Init(); // inits window 0 by default    

    // Rendering Lib
    SQUE_RENDER_Init();

    // If you use core lib, you will get these default initialization states per library
}

void SQUE_LIB_Close()
{
    // Call close for all required modules for SqueLib
    SQUE_RENDER_Close();
    SQUE_INPUT_Close();

    // Display is last that has ties with GLFW, holds GLFWTerminate
    SQUE_DISPLAY_Close(); 

    // Unrelated Helpers that are still required
    SQUE_LOGGER_Close();

#ifdef ANDROID
    ANativeActivity_finish(my_app->activity);
#endif
}
unsigned int SQUE_GetVersion(void)
{
    return SQUE_VERSION;
}

int SQUE_IsCompatibleDLL(void)
{
    uint32_t major = SQUE_GetVersion() >> 16;
    return major == SQUE_VERSION_MAJOR;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEFAULT LOGGING /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_PrintVargs(SQUE_LogType lt, const char file[], int line, const char* format, ...)
{
    std::string sttr(strrchr(file, FOLDER_ENDING));

    static va_list ap;
    char* tmp = new char[1];
    va_start(ap, format);
    int len = vsnprintf(tmp, 1, format, ap) + 1;
    va_end(ap);
    delete[] tmp;

    tmp = new char[len];
    va_start(ap, format);
    vsnprintf(tmp, len, format, ap);
    va_end(ap);

    int print_len = len + (sttr.length() + 4 + 4);
    char* print = new char[print_len];
    sprintf(print, "%s(%d): %s", sttr.c_str(), line, tmp);

    SQUE_ConsolePrint((int)lt, print);

    delete[] tmp;
    delete[] print;
}

void SQUE_ConsolePrint(int lt, const char* log)
{
#if defined(_WIN32) //&& defined(_DEBUG)
    OutputDebugString(log);
    OutputDebugString("\n");
#elif defined ANDROID
    __android_log_print(lt, "SqueLib", log);
#elif defined LINUX

#endif

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RANDOM NUMBER ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t SQUE_RNG()
{
    return pcg32_random();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// PERMISSIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef ANDROID
//Based on: https://stackoverflow.com/questions/41820039/jstringjni-to-stdstringc-with-utf8-characters
#include <jni.h>
#include <android/native_activity.h>
jstring android_permission_name(JNIEnv* env, const char* name)
{
    jclass class_manifest_permission = env->FindClass("android/Manifest$permission");
    jfieldID lid_PERM = env->GetStaticFieldID(class_manifest_permission, name, "Ljava/lang/String;");
    jstring ls_PERM = (jstring)(env->GetStaticObjectField(class_manifest_permission, lid_PERM));
    return ls_PERM;
}

// Android Permissions
// Based on: https://stackoverflow.com/questions/55062730/how-to-request-android-ndk-camera-permission-from-c-code
int AndroidHasPermissions(const char* name)
{
    JNIEnv* env = 0;
    JNIEnv** env_ptr = &env;
    JavaVM** jnii_ptr = &my_app->activity->vm;
    JavaVM* jnii = *jnii_ptr;

    jnii->AttachCurrentThread(env_ptr, NULL);
    env = (*env_ptr);
    int result = 0;
    jstring ls_PERM = android_permission_name(env, name);

    jint PERMISSION_GRANTED = -1;
    {
        jclass class_package_manager = env->FindClass("android/content/pm/PackageManager");
        jfieldID lid_PERMISSION_GRANTED = env->GetStaticFieldID(class_package_manager, "PERMISSION_GRANTED", "I");
        PERMISSION_GRANTED = env->GetStaticIntField(class_package_manager, lid_PERMISSION_GRANTED);
    }
    {
        jobject activity = my_app->activity->clazz;
        jclass class_context = env->FindClass("android/content/Context");
        jmethodID method_check_self_permission = env->GetMethodID(class_context, "checkSelfPermission", "(Ljava/lang/String;)I");
        jint res = env->CallIntMethod(activity, method_check_self_permission, ls_PERM);
        result = (res == PERMISSION_GRANTED);
    }
    jnii->DetachCurrentThread();

    return result;
}

void AndroidRequestAppPermissions(const char* perm)
{
    JNIEnv* env = 0;
    JNIEnv** env_ptr = &env;
    JavaVM** jnii_ptr = &my_app->activity->vm;
    JavaVM* jnii = *jnii_ptr;

    jnii->AttachCurrentThread(env_ptr, NULL);
    env = (*env_ptr);
    jobject activity = my_app->activity->clazz;

    jobjectArray perm_array = env->NewObjectArray(1, env->FindClass("java/lang/String"), env->NewStringUTF(""));
    env->SetObjectArrayElement(perm_array, 0, android_permission_name(env, perm));
    jclass class_activity = env->FindClass("android/app/Activity");

    jmethodID method_request_permissions = env->GetMethodID(class_activity, "requestPermissions", "([Ljava/lang/String;I)V");
    // Last arg is to send a callback if needed (not sure if needed for now, CNLohr does not)
    env->CallVoidMethod(activity, method_request_permissions, perm_array, 0);
    SQUE_PRINT(LT_INFO, "Asked for %s", perm);

    jnii->DetachCurrentThread();
}

#endif

int SQUE_AskPermissions(const char* permission_name)
{
    int ret = 1;
#ifdef ANDROID
    ret = AndroidHasPermissions(permission_name);
    if(ret > 0)
        return ret;
    AndroidRequestAppPermissions(permission_name);
    return AndroidHasPermissions(permission_name);
#endif

    return ret;    
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CALLBACKS / FLOW MANAGEMENT /////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VoidFun SQUE_AddOnResumeCallback(VoidFun fun)
{
    VoidFun ret = on_resume_callback;
    on_resume_callback = fun;
    return ret;

}
VoidFun SQUE_AddOnGoBackgroundCallback(VoidFun fun)
{
    VoidFun ret = on_go_background_callback;
    on_go_background_callback = fun;
    return ret;
}

void SQUE_Sleep(uint32_t ms)
{
#if defined(ANDROID) || defined(__linux__)
    usleep(ms * 1000);
#else if defined(_WIN32)
    Sleep(ms);
#endif
}