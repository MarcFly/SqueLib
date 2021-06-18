#ifndef _BLOCK_H_
#define _BLOCK_H_

#include "../entity.h"

class Block : public Entity
{
public:
	Block();
	Block(uint16_t _tier, glm::vec2 _pos, float size_mult = 1);
	~Block();

	uint16_t tier;
	uint16_t hp = 1;
	
	void OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos) override;
};

#endif