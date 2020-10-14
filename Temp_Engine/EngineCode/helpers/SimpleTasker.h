#ifndef _SimpleTasker_H_
#define _SimpleTasker_H_

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

#endif //_SimpleTasker_H_