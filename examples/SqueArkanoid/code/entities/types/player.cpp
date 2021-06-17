#include "player.h"
#include <squelib.h>

Player::Player()
{
	Init();
}


Player::~Player()
{

}

void Player::Init()
{
	tex_handle = 0;
	sound_handle = 0;

	int32_t wx, wy;
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	size.x = wx / 10;
	size.y = wy / 20;

	pos.x = wx / 2;
	pos.y = wy / 10;
}

void Player::Update(float dt)
{

}

void Player::CleanUp()
{

}

glm::vec2 Player::OnCollision(glm::vec2 dir_speed, uint32_t ball_state)
{
	return dir_speed;
}

