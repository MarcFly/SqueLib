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
	paddle_timer.Start();
	tex_handle = 0;
	sound_handle = 0;

	int32_t wx, wy;
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	size.x = wx / 10;
	size.y = wy / 20;

	pos.x = wx / 2;
	pos.y = wy / 10;
}

static float x, y;
static int32_t wx, wy;

void Player::Update(float dt)
{
	// Movement
	SQUE_INPUT_GetPointerPos(&x, &y);
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	if (x < pos.x - size.x/2)
		pos.x -= wx * dt;
	else if (x > pos.x + size.x/2)
		pos.x += wx * dt;

	// Correct if out of bounds
	if (pos.x + size.x / 2 > wx)
		pos.x -= pos.x + size.x / 2 - wx;
	else if (pos.x - size.x / 2 < 0)
		pos.x -= pos.x - size.x / 2;

	// ?

}

void Player::CleanUp()
{

}

void Player::OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos)
{
	bool test = false;
	if (c_pos.x < pos.x - size.x/3.)
		dir_speed->x = -1. * abs(dir_speed->x);
	if (c_pos.x > pos.x + size.x/3.)
		dir_speed->x = abs(dir_speed->x);
		
}

