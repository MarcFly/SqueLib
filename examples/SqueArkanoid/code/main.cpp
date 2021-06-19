 #include <squelib.h>
// Include submodules
#include "render/render.h"
#include "entities/entity.h"
#include "ui/ui.h"

#include <glm.hpp>

static float dt = 0;
static SQUE_Timer timer;
static ColorRGBA clear_col;


/* Organization:

- Render
    .Animated particles?
- Entity Types
    .Wall = Only bounce
    .Block = Score, HP
    .Bouncer = What makes the ball bounce
    .Ball = What bounces arounds and tests collisions
    .Powerups?
- Basic UI - ImGui, not my own by any means
    .Score
    .Level Selector?

*/

// -- 

static bool on_background = false;

static VoidFun* prev_bg;
static VoidFun* prev_resume;

void OnGoBackground()
{
    prev_bg();
    on_background = true;
    EntitiesCleanUp();
    UiCleanUp();
    RenderCleanUp();


}

void OnResume()
{
    prev_resume();
    on_background = false;
    EntitiesInit();
    RenderInit();
    UiInit();
}

int main(int argc, char** arv)
{
    SQUE_LIB_Init("SqueArkanoid", SQ_INIT_DEFAULTS | SQ_INIT_DEBUG_RENDER);
    clear_col = ColorRGBA(0, 0, 0, 1);
    timer.Start();

    prev_bg = SQUE_AddOnGoBackgroundCallback(OnGoBackground);
    prev_resume = SQUE_AddOnResumeCallback(OnResume);

    // Init Submodules
    EntitiesInit();
    RenderInit();
    UiInit();

    while(!SQUE_DISPLAY_ShouldWindowClose())
    {
        if(on_background)
        {
            SQUE_Sleep(100);
            timer.Start();
            continue;
        }

        dt = timer.ReadMilliSec() / 1000.;
        timer.Start();
        dt = std::clamp(dt, 0.f, 1 / 30.f);
        SQUE_RENDER_Clear(clear_col, SQUE_DEPTH_BIT | SQUE_COLOR_BIT);
        SQUE_INPUT_Process(0);

        UiPreUpdate();
        
        EntitiesUpdate(dt);
        RenderUpdate(dt);
        // Render UI
        UiRender();

        // App Render
        SQUE_CHECK_RENDER_ERRORS();
        SQUE_DISPLAY_SwapAllBuffers();
    }

    // CleanUp Submodules
    EntitiesCleanUp();
    RenderCleanUp();
    UiCleanUp();

    SQUE_LIB_Close();
    
    return 0;
}