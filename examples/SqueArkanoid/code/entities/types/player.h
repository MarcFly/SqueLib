#ifndef _PLAYER_H_
#define _PLAYER_H_

#include "../entity.h"

class Player : public Entity
{
public:
	Player();
	~Player();

	SQUE_Timer paddle_timer;

	void Init() override;
	void Update(float dt) override;
	void CleanUp() override;

	void OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos) override;
};



#endif