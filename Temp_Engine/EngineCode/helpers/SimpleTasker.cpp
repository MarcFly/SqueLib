#include "SimpleTasker.h"
#include <atomic>

namespace ST
{
    std::once_flag init;
    std::once_flag close;

    std::vector<std::thread> threads;

    std::queue<Task*> scheduledTasks;
    std::mutex _mtx;
    std::condition_variable ev;
    std::atomic<bool> exitFlag = false;
}

using namespace ST;

void SimpleTasker::SetExitFlag()
{
    exitFlag = true;
    ev.notify_all();
}

void threadMain()
{
    Task* task = nullptr;

    while (true)
    {
        {
            std::unique_lock<std::mutex> lock(_mtx);
            while (scheduledTasks.size() == 0 && exitFlag == false)
            {
                ev.wait(lock);
            }

            if (exitFlag) break;

            task = scheduledTasks.front();
            scheduledTasks.pop();
        }

        task->run();

        delete task;
    }
}

void SimpleTasker::INIT()
{
    std::call_once(init, []() {

        int num_threads = std::thread::hardware_concurrency() - 1;

        while (threads.size() < num_threads)
            threads.push_back(std::thread(threadMain));

    });
}



void SimpleTasker::addTask(Task* task)
{
    std::lock_guard<std::mutex> lock(_mtx);
    scheduledTasks.push(task);
    ev.notify_all();
}

void SimpleTasker::CLOSE()
{
    std::call_once(close, []() {

        SetExitFlag();

        for (int i = 0; i < threads.size(); ++i)
            threads[i].join();

        threads.clear();

        while (scheduledTasks.size() > 0)
        {
            delete scheduledTasks.front();
            scheduledTasks.pop();
        }
    });
}