#include <iostream>

#include <fly_lib.h>

enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

int main()
{
    main_states state = MAIN_CREATION;
    bool ret = false;

    // Helpers Initialization
    {
        ret = FLYLIB_Init(/*flags*/);
    }

    // Engine Initialization
    {
        FLYLOG(FlyLogType::LT_INFO, "Initializing Engine...");
        // Initialize all modules in required order

        state = (ret) ? MAIN_UPDATE : MAIN_EXIT;

        if (ret) { FLYLOG(FlyLogType::LT_INFO, "Entering Update Loop..."); }
        else FLYLOG(FlyLogType::LT_WARNING, "Error Initializing Engine...");
    }

    // Update Loop
    while(state == MAIN_UPDATE)
    {
        // Update all modules in specific order
        state = MAIN_FINISH;
    }

    //  Engine CleanUp
    {
        FLYLOG(FlyLogType::LT_INFO, "Cleaning Up Engine...");

        // perform the CleanUp of all modules in reverse init order preferably

        if (ret) { FLYLOG(FlyLogType::LT_INFO, "Application Cleanup Performed Successfully..."); }
        else FLYLOG(FlyLogType::LT_WARNING, "Application Cleanup with errors...");

        // Delete memory bound modules
    }

    // Helpers CleanUp
    {
        FLYLOG(FlyLogType::LT_INFO, "Finishing Executing Engine...");
        // Close something that is not part of the engine as a module

        FLYLOG(FlyLogType::LT_INFO, "Closing Helpers...");
        FLYLIB_Close();
    }

    return 0;
}