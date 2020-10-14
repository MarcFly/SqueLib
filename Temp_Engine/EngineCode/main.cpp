#include <iostream>;
#include <conio.h>

#include "helpers/Globals.h"
#include "application/Application.h"

enum main_states
{
	MAIN_CREATION,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main(int argc, char* argv[])
{
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
        LOGGER::ILOG("Closing Helpers...")
            SimpleTasker::CLOSE();

        LOGGER::ILOG("Finishing Executing Engine...");
        LOGGER::CLOSE(true);
    }

    getch();
    return 0;
}