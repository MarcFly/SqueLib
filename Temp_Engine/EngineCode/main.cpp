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

struct LogTask : public Task
{

    int logN;

    LogTask(int n) : logN(n) {};

    void run()
    {
        LOGGER::ILOG("Task Doing Log Number %d", logN);
    }
};

int main(int argc, char* argv[])
{
    LOGGER::INIT();
    LOGGER::ILOG("Starting Engine...");
    SimpleTasker::INIT();

    main_states state = MAIN_CREATION;

    LOGGER::ILOG("Creating Engine...");    

    state = MAIN_UPDATE;
    while(state == MAIN_UPDATE)
    {
        state = MAIN_FINISH;
    }

    LOGGER::ILOG("Cleaning Up Engine...");

    LOGGER::ILOG("Closing Simple Profiler Subsystem...")

    LOGGER::ILOG("Closing Simple Tasker Subsystem...")
    SimpleTasker::CLOSE();
    SimpleTasker::CLOSE();

    LOGGER::ILOG("Finishing Executing Engine...")

    LOGGER::CLOSE(true);

    getch();
    return 0;
}