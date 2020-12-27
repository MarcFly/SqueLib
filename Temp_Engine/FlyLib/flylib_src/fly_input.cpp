#ifdef INPUT_SOLO
#   include "fly_input.h"
#else
#   include "fly_lib.h"
#endif

#ifdef ANDROID

#include <android_native_app_glue.h>

int OGLESStarted = 0;

void handle_android_cmd(struct android_app* app, int32_t cmd)
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

int32_t handle_android_input(struct android_app* app, AInputEvent* ev)
{

    return 0;
}

#endif