#include "ModuleTime.h"

//------------------------------------------------------------------------------------------------------------
// Override Virtuals
//------------------------------------------------------------------------------------------------------------

ModuleTime::ModuleTime()
{
    name = "ModuleTime";
}

ModuleTime::~ModuleTime()
{
    delete name;
}

bool ModuleTime::Init()
{
    bool ret = true;

    return ret;
}

bool ModuleTime::Update(float engine_dt, float game_dt)
{
    bool ret = true;

    CalcDT();
    
    return ret;
}

bool ModuleTime::CleanUp()
{
    bool ret = true;

    return ret;
}

//------------------------------------------------------------------------------------------------------------
// Time Functions
//------------------------------------------------------------------------------------------------------------

float ModuleTime::CalcDT()
{
    float last_frame = 1000*timer.Read_mS();
    timer.Start();

    engine_dt = (last_frame < max_dt) ? last_frame : max_dt;
    game_dt = engine_dt * control_timings[control];

    return engine_dt;
}

//------------------------------------------------------------------------------------------------------------
// Event Response Functions
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
// Var Access/Modification Functions
//------------------------------------------------------------------------------------------------------------

void ModuleTime::SetTimeControl(TIME_SCALE _control)
{
    control = _control;
}
void ModuleTime::SetMaxDT(float min_dt)
{
    max_dt = min_dt;
}