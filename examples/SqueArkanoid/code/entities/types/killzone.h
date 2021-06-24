#ifndef _KILLZONE_H_
#define _KILLZONE_H_

#include "../entity.h"

class KillZone : public Entity
{
public:
	KillZone();
	~KillZone();

	void OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos) override;
};


#endif