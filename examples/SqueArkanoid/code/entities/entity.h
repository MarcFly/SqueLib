#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <squelib.h>
#include <glm.hpp>
#include <soloud.h>
#include <soloud_wav.h>

class Entity
{
public:
    Entity() {};
    ~Entity() {};

    glm::vec2 pos;
    glm::vec2 size;

    uint32_t tex_handle;
    uint32_t sound_handle;

    virtual void Init() {}
    virtual void Update(float dt) {}
    virtual void CleanUp() {}
    virtual glm::vec2 OnCollision(glm::vec2 dir_speed, uint32_t ball_state) { return dir_speed; }
    // Ball state is what color went before -> effects, bonus points,...
    // Vector returs depends on the block and interaction between colors

    void PlaySound();

};

void EntitiesInit();
void EntitiesUpdate(float dt);
void EntitiesCleanUp();

void EntitiesAdd(Entity* e);
void EntitiesClear();
const sque_vec<Entity*>& EntitiesGet();

#endif