#include <iostream>;
#include <conio.h>

#include "./test.h"
#include "helpers/Globals.h"

enum main_states
{
	MAIN_CREATION,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};


int v;

int main(int argc, char* argv[])
{
    LOGGER::ILOG("Starting Engine...");
    SProf::SProfiler_INIT(true);
    LOGGER::LH_INIT();

    int main_return = EXIT_FAILURE;
    main_states state = MAIN_CREATION;

    LOGGER::ILOG("Creating Engine...");    

    int i = 0;
    state = MAIN_UPDATE;
    while(state == MAIN_UPDATE)
    {
        LOGGER::ILOG("Entering update loop n.%d...", i);

        ++i;
        if(i > 10) state = MAIN_FINISH;
    }

    LOGGER::ILOG("Cleaning Up Engine...");

    LOGGER::ILOG("Finishing Executing Engine...");

    LOGGER::LH_CLOSE(true);
    getch();
    return 0;
}