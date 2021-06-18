#include "entity.h"

static sque_vec<Entity*> background;
static sque_vec<Entity*> entities;

// Separate audio into new module?
static SoLoud::Soloud audio_master;
static sque_vec<SoLoud::Wav> sounds;
static sque_vec<SoLoud::Wav*> sound_queue;
// Load Level...

void Entity::PlaySound()
{
	sound_queue.push_back(&sounds[sound_handle]);
}

void EntitiesInit()
{
	audio_master.init();
	// Load all sounds
}

void EntitiesUpdate(float dt)
{
	for (uint32_t i = 0; i < entities.size(); ++i)
		entities[i]->Update(dt);

	for (uint32_t i = 0; i < sound_queue.size(); ++i)
		audio_master.play(*sound_queue[i]);
	sound_queue.clear();

	for (uint32_t i = 0; i < entities.size(); ++i)
	{
		if (entities[i]->to_delete)
		{
			SQUE_Swap(&entities[i], entities.last());
			entities.pop_back();
			--i;
		}
	}
}

void EntitiesCleanUp()
{
	for (uint32_t i = 0; i < entities.size(); ++i)
		entities[i]->CleanUp();
}

void EntitiesAdd(Entity* e)
{
	entities.push_back(e);
}

void EntitiesAddBackground(Entity* e)
{
	background.push_back(e);
}

void EntitiesClear()
{
	for (uint32_t i = 0; i < entities.size(); ++i)
		delete entities[i];
	entities.clear();
}

const sque_vec<Entity*>& EntitiesGet()
{
	return entities;
}

const sque_vec<Entity*>& EntitiesGetBg()
{
	return background;
}