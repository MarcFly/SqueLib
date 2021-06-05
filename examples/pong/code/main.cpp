#include <squelib.h>
#include <math.h>
#include <cmath>
#include <cstring>
#include <glm.hpp>
    

// Audio Init basic
#include <soloud.h>
#include <soloud_wav.h>

SoLoud::Soloud audio_master;
SoLoud::Wav audio_source;

void InitAudio()
{
    audio_master.init();
    SQUE_Asset* beep_sound = SQUE_FS_LoadAssetRaw("beep.wav");
    audio_source.loadMem((uchar*)beep_sound->raw_data, beep_sound->size);
}

// Pong
#include <code/objects.h>
#include <code/render.h>
#include <code/collision.h>

bool on_background = false;
void OnResume()
{
    InitParams();
    InitShaders();
    on_background = false;
}
void OnGoBackground()
{
    on_background = true;
}

extern Paddle player1;
extern Paddle player2;
extern Ball ball;

void CheckResizeScreen(int32_t sizex, int32_t sizey)
{
    if ((int)player1.x != sizex / 40 || (int)player2.x != (sizex-sizex/40))
    {
        player1.x = sizex / 40.;
        player2.x = sizex - sizex / 40.;

        player1.sizex = sizex / 80.;
        player2.sizex = sizex / 80.;
        ball.radius = sizex / 50.;
    }
    if (((int)player1.sizey) != (sizey / 8))
    {
        player1.sizey = sizey / 8.;
        player2.sizey = sizey / 8.;
        ball.y = sizey / 2.;
        ball.radius = (sizey / 50.);
    }
}   

void ActivePointerCallback(int32_t code, int32_t state)
{
    SQUE_INPUT_SetPointerActive(code, (state > SQUE_ACTION_RELEASE));
}

void MovePaddles(float dt)
{
    float x1,y1;
    float x2,y2;
    int sx, sy;

    SQUE_DISPLAY_GetWindowSize(&sx, &sy);

    // Pointer Based - Android
    bool p1 = SQUE_INPUT_GetPointerPos(&x1, &y1, 0);
    bool p2 = SQUE_INPUT_GetPointerPos(&x2, &y2, 1);
    if (p1)
    {
        if(x1 <= sx /2)
            player1.y = sy - (y1);
        else
            player2.y = sy - (y2);
    }

    if (p2)
    {
        if(x2 <= sx /2)
            player1.y = sy - (y1);
        else
            player2.y = sy - (y2);
    }

    // Keyboard Based - Desktop
    float p_speed = sy / 2. * dt;
    if (SQUE_INPUT_GetKey(SQUE_KEY_ARROW_UP) > SQUE_ACTION_RELEASE) player2.y += p_speed;
    if (SQUE_INPUT_GetKey(SQUE_KEY_ARROW_DOWN) > SQUE_ACTION_RELEASE) player2.y -= p_speed;
    if (SQUE_INPUT_GetKey(SQUE_KEY_UPPER_W) > SQUE_ACTION_RELEASE) player1.y += p_speed;
    if (SQUE_INPUT_GetKey(SQUE_KEY_UPPER_S) > SQUE_ACTION_RELEASE) player1.y -= p_speed;

    player1.y = (player1.y - player1.sizey/2. < 0) ? 0 + player1.sizey / 2. : player1.y;
    player1.y = (player1.y + player1.sizey / 2. > sy) ? sy - player1.sizey / 2. : player1.y;
    player2.y = (player2.y - player1.sizey / 2. < 0) ? 0 + player1.sizey / 2. : player2.y;
    player2.y = (player2.y + player1.sizey / 2. > sy) ? sy - player1.sizey / 2. : player2.y;    
}

int main(int argc, char** argv)
{
    SQUE_LIB_Init("SquePong");
    InitAudio();

    SQUE_AddOnGoBackgroundCallback(OnGoBackground);
    SQUE_AddOnResumeCallback(OnResume);

    InitGLDebug();

    SQUE_DISPLAY_SetVSYNC(0);

    SQUE_INPUT_SetMouseButtonCallback(0, ActivePointerCallback);
    SQUE_INPUT_SetMouseButtonCallback(1, ActivePointerCallback);
    
    InitParams();
    InitShaders();

    double last_time = 0;
    SQUE_Timer timer;

    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        SQUE_INPUT_Process(0);

        float dt = timer.ReadMilliSec() / 1000.;
        MovePaddles(dt);
        if(on_background)
        {
            SQUE_Sleep(100);
            continue;
        }
        
        timer.Start();

        // Update things
        ball.x += ball.dirx*dt*ball.speed;
        ball.y += ball.diry*dt*ball.speed;
        BallCollisionWalls(dt, ball);
        BallCollisionPaddle(dt, player1, ball);
        BallCollisionPaddle(dt, player2, ball);

        // Render things
        SQUE_RENDER_Clear(col);

        // Ball Render
        
        int vx, vy;
        SQUE_DISPLAY_GetWindowSize(&vx, &vy);
        CheckResizeScreen(vx, vy);
        SQUE_DISPLAY_MakeContextMain(0);

        BallRender(vx, vy);
        PlayersRender(vx, vy);
        
        SQUE_CHECK_RENDER_ERRORS();
        SQUE_DISPLAY_SwapAllBuffers();
    }
    
    audio_master.deinit();
    SQUE_LIB_Close();

    return 0;
}