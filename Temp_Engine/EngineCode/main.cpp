#include <iostream>
#include "EngineCode/helpers/Globals.h"

#ifdef ANDROID
#include "EngineCode/android/application/Application.h"
#include <android_native_app_glue.h>
//app_dummy();
extern struct android_app* gapp;
#else
#include "EngineCode/application/Application.h"
#endif


enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

void HandleResume()
{}

void HandleSuspend()
{}

int main()
{
#ifdef ANDROID
    //app_dummy();
#endif

    app = new Application();
    main_states state = MAIN_CREATION;
    bool app_rets = false;

    // Helpers Initialization
    {
        SimpleTasker::INIT();
        LOGGER::INIT();
    }

    // Engine Initialization
    {
        LOGGER::ILOG("Initializing Engine...");
        app_rets = app->Init();

        state = (app_rets) ? MAIN_UPDATE : MAIN_EXIT;

        if (app_rets) { LOGGER::ILOG("Entering Update Loop..."); }
        else LOGGER::WLOG("Error Initializing Engine...");
    }

    // Update Loop
    while(state == MAIN_UPDATE)
    {
        app->Update();
        state = MAIN_FINISH;
    }

    //  Engine CleanUp
    {
        LOGGER::ILOG("Cleaning Up Engine...");

        app_rets = app->CleanUp();

        if (app_rets) { LOGGER::ILOG("Application Cleanup Performed Successfully..."); }
        else LOGGER::WLOG("Application Cleanup with errors...");

        delete app;
    }

    // Helpers CleanUp
    {
        LOGGER::ILOG("Closing Helpers...");
        SimpleTasker::CLOSE();

        LOGGER::ILOG("Finishing Executing Engine...");
        LOGGER::CLOSE(true);
    }

    return 0;
}