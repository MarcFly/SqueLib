#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

struct Task
{
    virtual void run() {};
};

namespace SimpleTasker
{
    void INIT();

    void CLOSE();

    void addTask(Task* task);   

    void SetExitFlag();
}