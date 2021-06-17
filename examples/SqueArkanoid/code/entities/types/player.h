#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "../entity.h"

class Player : public Entity
{
public:
	Player();
	~Player();

	void Init();
	void Update(float dt);
	void CleanUp();

	glm::vec2 OnCollision(glm::vec2 dir_speed, uint32_t ball_state);
};



#endif