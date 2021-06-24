#ifndef _UI_SCENE1_H_
#define _UI_SCENE1_H_

#include "../ui.h"

class PlayScene : public UI_Item
{
public:
    PlayScene();
    ~PlayScene();

    void Init();
    void Setup(uint32_t level);
    void Update();

    void CleanUp();

    float GetHiTime() { return high_time; }

private:
    void SetupDemo();
    void SetupSurvival();
    bool survival = false;
    float high_time = 0;
    SQUE_Timer surv_timer;
};

#endif