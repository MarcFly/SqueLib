#ifndef _Module_Time_H_
#define _Module_Time_H_

#include "Module.h"
#include "EngineCode/helpers/LWTimer.h"

enum TIME_SCALE
{
    REVERSE_FAST = 0,
    REVERSE,
    REVERSE_SLOW,
    PAUSE,
    STOP,
    PLAY,
    SLOW,
    FAST,
    CUSTOM,
    MAX
};

class ModuleTime : public Module
{
public:

    // Override Virtuals
    ModuleTime();
    ~ModuleTime();

    bool Init() override;
    bool Update(float engine_dt, float game_dt) override;
    bool CleanUp() override;

    // Time Functions
    float CalcDT();

    // Event Response Functions

    // Var Access Functions
    void SetTimeControl(TIME_SCALE _control);
    void SetMaxDT(float min_dt);
    
    float GetGameDT() const { return game_dt; };
    float GetEngineDT() const { return engine_dt; };
    float GetFixedDT() const { return fixed_dt; };
    float GetControlTiming() const { return control_timings[control]; };

public:

private:
    // Delta Time Expressed in Seconds
    std::atomic<float> max_dt = 1;
    std::atomic<float> game_dt = 0;
    std::atomic<float> engine_dt = 0;
    std::atomic<float> fixed_dt = 1/60.f; // I want fixed_dt to match Screen Refresh Rate, will update later on window Module
    TIME_SCALE control = PLAY;
    float control_timings[TIME_SCALE::MAX];
    LW_Timer timer;
    

};

#endif // _Module_Time_H_