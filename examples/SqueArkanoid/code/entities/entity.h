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

    bool to_delete = false;
    glm::vec3 col = { 1,1,1 };
    glm::vec2 pos = { 0,0 };
    glm::vec2 size = { 10,10 };

    uint32_t tex_handle = 0;
    uint32_t sound_handle = 0;

    virtual void Init() {}
    virtual void Update(float dt) {}
    virtual void CleanUp() {}
    virtual void OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos) {}

    void PlaySound();
};

void EntitiesInit();
void EntitiesUpdate(float dt);
void EntitiesCleanUp();

void EntitiesAdd(Entity* e);
void EntitiesAddBackground(Entity* e);
void EntitiesClear();
const sque_vec<Entity*>& EntitiesGet();
const sque_vec<Entity*>& EntitiesGetBg();

// Game things
void AddScore(uint32_t score);
uint32_t GetScore();
void ModifyLives(int32_t change);
uint32_t GetLives();
uint32_t GetHiscore();
uint32_t GetSHiscore();
void ToggleSurvival();


#endif