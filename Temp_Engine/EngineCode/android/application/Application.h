#ifndef _Application_H_
#define _Application_H_

#include<vector>
#include<atomic>
#include "EngineCode/helpers/Globals.h"
#include "EngineCode/modules/Module.h"
#include <android_native_app_glue.h>

class ModuleTime;

class Application{

public:
    Application();
    ~Application();

    bool Init();
    bool Update();
    bool CleanUp();

public:
    ModuleTime* time = nullptr;

private:
    std::vector<Module*> modules;
    std::atomic<float> engine_time;
    std::atomic<float> game_time;

};
extern struct android_app* gapp;
static Application* app = nullptr;

#endif // _Application_H_