#ifndef _BALL_H_
#define _BALL_H_

#include "../entity.h"

class Ball : public Entity
{
public:
	Ball();
	~Ball();

	glm::vec2 dir;
	float speed;
	uint32_t ball_state;

	SQUE_Timer respawn_timer;

	void Init() override;
	void Update(float dt) override;
	void CleanUp() override;
};

#endif