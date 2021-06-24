#include "block.h"

Block::Block()
{

}

Block::~Block()
{

}

Block::Block(uint16_t _tier, glm::vec2 _pos, float size_mult)
{
	tier = hp = _tier;
	tex_handle = tier + 2;
	sound_handle = 1;

	// Calculate size for about 20 total vertical tiles
	int32_t wx, wy;
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	size.y = wy / 21.;
	size.x = size.y * size_mult;

	float cap_top = wx / (wy / 21.);
	float min_ratio = (uint32_t)cap_top;
	min_ratio = cap_top / (float)(uint32_t)cap_top;
	size.x *= min_ratio;
	to_delete = (_pos.x > cap_top + 1) || (_pos.x < 0);

	glm::vec2 half(size.y / 2.);
	pos = _pos * size;
}

void Block::OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos)
{
	if (hp == 1)
	{
		AddScore(tier * 100);
		to_delete = true;
		sound_handle = 2;
	}
	--hp;
	if(tex_handle > 2)
		--tex_handle;
	// State change depending on tier and current ball_state...
	switch (tier)
	{

	}
}

// all functions for state...
