#ifndef _SQUEPONG_OBJECTS_
#define _SQUEPONG_OBEJCTS_

#include <squelib.h>

float quad[] = {
    1,  1,
    1, -1,
    -1, -1,
    -1,  1
};

uint32_t quad_indices[]{
    0,1,3,
    1,2,3
};

struct Paddle {
    float x, y;
    int sizex, sizey;

    SQUE_Mesh rect;
};

struct Ball {
    float x, y;
    int radius;
    float speed;
    float dirx = 1, diry = 1;

    SQUE_Mesh rect;
};

Ball ball;
Paddle player1;
Paddle player2;

void InitParams()
{
// BALL
    int vx, vy;
    SQUE_DISPLAY_GetViewportSize(&vx, &vy);
    ball.x = (float)vx / 2.f;
    ball.y = (float)vy / 2.f;
    ball.radius = vx/50.;
    ball.speed = 300;
    // Fault 2 -> Mesh declarations exaggeratedly verbose
    // function that inits: draw_config, draw_mode
    // function that sets: verts + vert_num,  index+index_var+index_num
    // 3 Functions but it makes more sense in the long run (variable verts, variable index, variable draw_config/mode)
    SQUE_MESH_SetDataConfig(&ball.rect, 4, 6, SQUE_UINT);
    SQUE_MESH_SetDrawConfig(&ball.rect, SQUE_TRIANGLES, SQUE_STATIC_DRAW);

    
    SQUE_MESH_GenBuffer(&ball.rect);
    SQUE_MESH_BindBuffer(ball.rect);
    SQUE_MESH_AddAttribute(&ball.rect, "vertPos", SQUE_FLOAT, false, 2);
    SQUE_MESH_SetLocations(&ball.rect);
    SQUE_MESH_SendToGPU(ball.rect, quad, quad_indices);

// PLAYER 1
    player1.sizex = ball.y / 20;
    player1.sizey = ball.y / 4;
    player1.x = ball.x/20;
    player1.y = vy / 2;
    SQUE_MESH_SetDataConfig(&player1.rect, 4, 6, SQUE_UINT);
    SQUE_MESH_SetDrawConfig(&player1.rect, SQUE_TRIANGLES, SQUE_STATIC_DRAW);

    
    SQUE_MESH_GenBuffer(&player1.rect);
    SQUE_MESH_BindBuffer(player1.rect);
    SQUE_MESH_AddAttribute(&player1.rect, "vertPos", SQUE_FLOAT, false, 2);
    SQUE_MESH_SetLocations(&player1.rect);
    SQUE_MESH_SendToGPU(player1.rect, quad, quad_indices);
    

// PLAYER 2
    player2.sizex = player1.sizex;
    player2.sizey = player1.sizey;
    player2.x = vx - ball.x/20;
    player2.y = player1.y;
    SQUE_MESH_SetDataConfig(&player2.rect, 4, 6, SQUE_UINT);
    SQUE_MESH_SetDrawConfig(&player2.rect, SQUE_TRIANGLES, SQUE_STATIC_DRAW);

    
    SQUE_MESH_GenBuffer(&player2.rect);
    SQUE_MESH_BindBuffer(player2.rect);
    SQUE_MESH_AddAttribute(&player2.rect, "vertPos", SQUE_FLOAT, false, 2);
    SQUE_MESH_SetLocations(&player2.rect);
    SQUE_MESH_SendToGPU(player2.rect, quad, quad_indices);
    
}


#endif