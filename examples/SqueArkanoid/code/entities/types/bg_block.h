#ifndef _BG_BLOCK_H_
#define _BG_BLOCK_H_

#include "../entity.h"

class BgBlock : public Entity
{
public:
	BgBlock() {};
	BgBlock(uint16_t _tier, glm::vec2 _pos, float size_mult = 1)
	{
		tex_handle = _tier + 2;
		sound_handle = 3;
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

		col = { .3,.3,.3 };
	};
	~BgBlock() {};

	
};

#endif