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

int main(int argc, char* argv[])
{
    LOGGER::ILOG("Starting Engine...");
    SProf::SProfiler_INIT(true);
    LOGGER::LH_INIT();

    int main_return = EXIT_FAILURE;
    main_states state = MAIN_CREATION;

    /*while(state != MAIN_EXIT)
    {
        
    }*/

    


    LOGGER::LH_CLOSE(true);
    getch();
    return 0;
}