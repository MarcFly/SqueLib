#ifndef _Application_H_
#define _Application_H_

#include<vector>
#include<atomic>
#include "../helpers/Globals.h"

class Module;

class Application{

public:
    Application();
    ~Application();

    bool Init();
    bool Update();
    bool CleanUp();

private:
    std::vector<Module*> modules;
    std::atomic<uint32> engine_time = 1/60.f;
    std::atomic<uint32> game_time = 1/60.f;

};

static Application* app = nullptr;

#endif // _Application_H_