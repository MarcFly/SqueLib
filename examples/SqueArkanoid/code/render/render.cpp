#include "render.h"
#include <glm.hpp>

static float quad[] = {
    1,  1,
    1, -1,
    -1, -1,
    -1,  1
};

static uint32_t indices[]{
    0,1,3,
    1,2,3
};

static RenderData render;

void RenderLoadTex(const char* asset)
{
    render.textures.push_back(SQUE_Texture());
    SQUE_Texture* tex = render.textures.last();

    SQUE_TEXTURE_GenBufferIDs(1, &tex->id);
    SQUE_TEXTURE_SetFormat(tex, SQUE_TEXTURE_2D, SQUE_RGBA, SQUE_RGBA, SQUE_UBYTE);
    //SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0 + render.textures.size() + 1);
    SQUE_TEXTURE_Bind(tex->id, tex->dim_format);

    SQUE_TEXTURE_AddAttribute(tex, "min_filter", SQUE_MIN_FILTER, SQUE_NEAREST);
    SQUE_TEXTURE_AddAttribute(tex, "mag_filter", SQUE_MAG_FILTER, SQUE_LINEAR);
    SQUE_TEXTURE_ApplyAttributes(*tex);

    SQUE_Asset* temp_tex = SQUE_FS_LoadAssetRaw(asset);
    SQUE_LOAD_Texture(temp_tex, tex);
    SQUE_TEXTURE_SendAs2DToGPU(*tex, temp_tex->raw_data);
    SQUE_FREE_Texture(temp_tex);
    delete temp_tex;
}

void RenderInit()
{
    // Shaders
    SQUE_Asset* temp_v = SQUE_FS_LoadAssetRaw("vert_shader.sq_vert");
    SQUE_Asset* temp_f = SQUE_FS_LoadAssetRaw("frag_shader.sq_frag");
    SQUE_PROGRAM_GenerateID(&render.program.id);
    SQUE_Shader vert;
    SQUE_Shader frag;
    SQUE_SHADERS_GenerateID(&vert, SQUE_VERTEX_SHADER);
    SQUE_SHADERS_GenerateID(&frag, SQUE_FRAGMENT_SHADER);
    SQUE_SHADERS_SetSource(vert.id, temp_v->raw_data);
    delete temp_v->raw_data;
    delete temp_v;
    SQUE_SHADERS_SetSource(frag.id, temp_f->raw_data);
    delete temp_f->raw_data;
    delete temp_f;
    SQUE_SHADERS_Compile(vert.id);
    SQUE_SHADERS_Compile(frag.id);
    SQUE_PROGRAM_AttachShader(&render.program, vert);
    SQUE_PROGRAM_AttachShader(&render.program, frag);
    SQUE_PROGRAM_Link(render.program.id);
    SQUE_PROGRAM_Use(render.program.id);
    SQUE_PROGRAM_CacheUniforms(&render.program);

    // Quad Descriptor
    SQUE_MESH_GenBuffer(&render.quad_descriptor);
    SQUE_MESH_SetDataConfig(&render.quad_descriptor, 4, 6, SQUE_UINT);
    SQUE_MESH_SetDrawConfig(&render.quad_descriptor, SQUE_TRIANGLES, SQUE_STATIC_DRAW);

    SQUE_MESH_BindBuffer(render.quad_descriptor);
    SQUE_MESH_AddAttribute(&render.quad_descriptor, "vertPos", SQUE_FLOAT, false, 2);
    SQUE_MESH_InterleaveOffsets(&render.quad_descriptor);
    SQUE_MESH_SetLocations(&render.quad_descriptor);
    SQUE_MESH_SendToGPU(render.quad_descriptor, quad, indices);

    // Texture Atlas
    // Ordered by handle logic
    RenderLoadTex("2d/paddleRed.png");
    RenderLoadTex("2d/ballBlue.png");
    RenderLoadTex("2d/element_grey_rectangle.png");
    RenderLoadTex("2d/element_blue_rectangle.png");
    RenderLoadTex("2d/element_yellow_rectangle.png");
    RenderLoadTex("2d/element_red_rectangle.png");
    RenderLoadTex("2d/element_purple_rectangle.png");
    
      
}

#include "../entities/entity.h"
#include <glm/gtc/type_ptr.hpp>

void RenderUpdate(float dt)
{
    render.state.BackUp();

    SQUE_PROGRAM_Use(render.program.id);

    // Uniform for all entities
    int32_t n1,n2,vx, vy;
    SQUE_RENDER_GetViewport(&n1, &n2, &vx, &vy);
    SetFloat2(SQUE_PROGRAM_GetUniformID(render.program, "vp"), glm::value_ptr(glm::vec2(vx,vy)));
    float red[3] = { 1,0,0 };
    SetFloat3(SQUE_PROGRAM_GetUniformID(render.program, "col"), red);
    //Loop through all entities and draw them all
    const sque_vec<Entity*>& entities = EntitiesGet();
    for (uint32_t i = 0; i < entities.size(); ++i)
    {
        // How to bind textures properly?
        const SQUE_Texture& tex = render.textures[entities[i]->tex_handle];
        uint32_t tex_unit = entities[i]->tex_handle + 1; // Unit 0 is for ImGui
        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);// + entities[i]->tex_handle+1);
        SQUE_TEXTURE_Bind(tex.id, tex.dim_format);

        SetInt(SQUE_PROGRAM_GetUniformID(render.program, "atlas"), 0);// entities[i]->tex_handle + 1);
        SetFloat2(SQUE_PROGRAM_GetUniformID(render.program, "center"), glm::value_ptr(entities[i]->pos));
        SetFloat2(SQUE_PROGRAM_GetUniformID(render.program, "size"), glm::value_ptr(entities[i]->size));
        
        SQUE_RENDER_DrawIndices(render.quad_descriptor);
    }

    render.state.SetUp();
}

void RenderCleanUp()
{
    for(uint32_t i = 0; i < render.textures.size(); ++i)
        SQUE_TEXTURE_FreeFromGPU(1, &render.textures[i].id);
    SQUE_MESH_FreeFromGPU(render.quad_descriptor);
}