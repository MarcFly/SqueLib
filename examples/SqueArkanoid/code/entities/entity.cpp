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

	SQUE_Asset* sound_load = SQUE_FS_LoadAssetRaw("sounds/paddle_bounce.wav");
	sounds.push_back(SoLoud::Wav());
	sounds.last()->loadMem((uchar*)sound_load->raw_data, sound_load->size);

	sound_load = SQUE_FS_LoadAssetRaw("sounds/block_bounce.wav");
	sounds.push_back(SoLoud::Wav());
	sounds.last()->loadMem((uchar*)sound_load->raw_data, sound_load->size);

	sound_load = SQUE_FS_LoadAssetRaw("sounds/block_destruct.wav");
	sounds.push_back(SoLoud::Wav());
	sounds.last()->loadMem((uchar*)sound_load->raw_data, sound_load->size);

	sound_load = SQUE_FS_LoadAssetRaw("sounds/wall_bounce.wav");
	sounds.push_back(SoLoud::Wav());
	sounds.last()->loadMem((uchar*)sound_load->raw_data, sound_load->size);
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

// Game things
static struct GameData
{
	uint32_t lives = 3;
	uint32_t score = 0;
	uint32_t hiscore = 0;

	bool survival = false;
	uint32_t s_hiscore = 0;
} gamedata;

void AddScore(uint32_t score)
{
	gamedata.score += score;

	if (gamedata.survival)
	{
		if (gamedata.s_hiscore < gamedata.score)
			gamedata.s_hiscore = gamedata.score;
	}
	else
	{
		if (gamedata.hiscore < gamedata.score)
			gamedata.hiscore = gamedata.score;
	}
}

uint32_t GetHiscore()
{
	return gamedata.hiscore;
}

uint32_t GetSHiscore()
{
	return gamedata.s_hiscore;
}

void ToggleSurvival()
{
	gamedata.survival = !gamedata.survival;
}

uint32_t GetScore()
{
	return gamedata.score;
}

uint32_t GetLives()
{
	return gamedata.lives;
}

void ModifyLives(int32_t change)
{
	gamedata.lives += change;
}