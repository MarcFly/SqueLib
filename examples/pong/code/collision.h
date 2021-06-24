#ifndef _SQUEPONG_COLLISION_
#define _SQUEPONG_COLLISION_

//#include <code/objects.h>

void BallCollisionWalls(float dt, Ball& b)
{
    int vx, vy;
    SQUE_DISPLAY_GetViewportSize(&vx, &vy);
    float bx = b.x + b.dirx*b.speed*dt;
    float by = b.y + b.diry*b.speed*dt;

    if((b.x - b.radius) < 0 || (b.x+b.radius) > vx) 
    {
        b.y = vy/2;
        b.x = vx/2;
        b.speed = 300;
        // random ball y dir
    }

    if((b.y - b.radius) < 0 || (b.y + b.radius) > vy)
    {
        b.diry *= -1;
        b.y += b.diry*b.speed*dt;
    }
}

static int32_t collision_downtime_ms = 200;
static SQUE_Timer collision_timer;
void BallCollisionPaddle(float dt, Paddle& p, Ball& b)
{
    if (collision_timer.ReadMilliSec() < collision_downtime_ms) return;
    // Left is in paddle
    float b_left = b.x - b.radius;
    float b_right = b.x + b.radius;
    float b_top = b.y - b.radius;
    float b_bottom = b.y + b.radius;

    float p_left = p.x - p.sizex;
    float p_right = p.x + p.sizex;
    float p_top = p.y - p.sizey;
    float p_bottom = p.y + p.sizey;
    bool y_in = (b_top < p_bottom && b_top > p_top) || (b_bottom < p_bottom && b_bottom > p_top);
    bool x_in = (b_left < p_right&& b_left > p_left) || (b_right < p_right&& b_right > p_left);
    

    if( y_in && x_in)
    {
        b.speed *= 1.1;
        collision_timer.Start();
        b.dirx *= -1;
        b.x += b.dirx*b.speed*dt;
        audio_master.play(audio_source);

        //if (b.y > p.y + 2*p.sizey / 3 || b.y < p.y - 2* p.sizey / 3)
        //{
        //    b.diry *= -1;
        //    b.y += b.diry * b.speed * dt;
        //}
    }
}

#endif