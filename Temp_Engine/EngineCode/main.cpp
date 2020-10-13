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
    SProf::INIT(true);
    SimpleTasker::INIT();

    main_states state = MAIN_CREATION;

    LOGGER::ILOG("Creating Engine...");    

    LW_Timer tested;
    LW_Timer tester;

    int i = 0;
    std::atomic<int> v = 0;
    state = MAIN_UPDATE;
    while(state == MAIN_UPDATE)
    {
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;
        SimpleTasker::addTask(new LogTask(v)); ++v;

        LOGGER::ILOG("Entering update loop n.%d...", i);
        
        

        ++i;
        if(i > 1000) state = MAIN_FINISH;
    }

    LOGGER::ILOG("Cleaning Up Engine...");

    LOGGER::ILOG("Closing Simple Profiler Subsystem...")
    SProf::CLOSE();

    LOGGER::ILOG("Closing Simple Tasker Subsystem...")
    SimpleTasker::CLOSE();
    SimpleTasker::CLOSE();

    LOGGER::ILOG("Finishing Executing Engine...")

    LOGGER::CLOSE(true);

    getch();
    return 0;
}