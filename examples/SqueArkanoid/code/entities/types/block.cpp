#include "block.h"

Block::Block()
{

}

Block::~Block()
{

}

Block::Block(uint16_t _tier, glm::vec2 _pos)
{
	tier = hp = _tier;
	tex_handle = tier + 2;
	sound_handle = tier + 2;

	// do some auto defining grid here
	// so that calling function only requires a pos
	int32_t wx, wy;
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	pos = glm::vec2(wx/2,wy-10);

	size = glm::vec2(10000, 100);
}

void Block::OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos)
{
	if (hp == 1)
		to_delete = true;
	--hp;
	// State change depending on tier and current ball_state...
	switch (tier)
	{

	}
}

// all functions for state...
