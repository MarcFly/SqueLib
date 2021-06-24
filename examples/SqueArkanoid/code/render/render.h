#ifndef _RENDER_H_
#define _RENDER_H_

#include <squelib.h>

struct RenderData
{
    SQUE_RenderState state;
    SQUE_Program program;
    SQUE_Mesh quad_descriptor;
    sque_vec<SQUE_Texture> textures;
};

void RenderLoadTex(const char* asset);

void RenderInit();
void RenderUpdate(float dt);
void RenderCleanUp();


#endif