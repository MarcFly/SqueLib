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

	pos.x = wx / 2 - size.x/2.;
	pos.y = wy / 10;
}

static float x, y;
static int32_t wx, wy;

void Player::Update(float dt)
{
	// Movement
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	float pix = wx/2. *dt;
	if(SQUE_INPUT_GetPointerPos(&x, &y))
	{
		if (x < wx/2.)
			pos.x -= pix;
		else if (x > wx/2.)
			pos.x += pix;
	}
	pos.x += -(SQUE_INPUT_GetKey(SQUE_KEY_ARROW_LEFT) > 0 )*pix + (SQUE_INPUT_GetKey(SQUE_KEY_ARROW_RIGHT) > 0)*pix; 
	
	// Correct out of bounds
	if (pos.x + size.x > wx)
		pos.x = wx - size.x;
	else if (pos.x < 0)
		pos.x = 0;
	// ?

}

void Player::CleanUp()
{

}

void Player::OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos)
{
	bool test = false;
	if (c_pos.x < pos.x + size.x/4.)
		dir_speed->x = -1. * abs(dir_speed->x);
	if (c_pos.x > pos.x + 3* size.x/4.)
		dir_speed->x = abs(dir_speed->x);

	*dir_speed *= glm::vec2(1.05);		
}

