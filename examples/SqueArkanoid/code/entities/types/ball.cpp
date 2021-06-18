#include "ball.h"
#include "../entity.h"

Ball::Ball()
{
	Init();
}

Ball::~Ball()
{

}

void Ball::Init()
{
	tex_handle = 1;

	speed = 0;
	dir = glm::vec2(0, -1);

	int32_t wx, wy;
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	size.x = wx / 40;
	size.y = size.x;

	Entity* p = EntitiesGet()[0];
	pos = p->pos + glm::vec2(0, size.y/2 + p->size.y/2.);

	ball_state = 1;
}

void Ball::Update(float dt)
{
	if (speed == 0)
	{
		Entity* p = EntitiesGet()[0];
		pos = p->pos + glm::vec2(p->size.x /2. - size.x/2., size.y / 2 + p->size.y / 2.);

		double time = respawn_timer.ReadMilliSec() / 1000.;
		if (time > 3 || SQUE_INPUT_GetKey(SQUE_KEY_SPACE) == SQUE_ACTION_PRESS || SQUE_INPUT_GetMouseButton(SQUE_MOUSE_LEFT) == SQUE_ACTION_PRESS)
		{
			speed = 500;
			respawn_timer.Kill();
			// min angle -60` max angle 60`
			// time < .3 == -60, time > 1.9 == 60

			dir.x = (glm::clamp(time, .5, 2.5) - .5) / 2.;
			dir.x = (dir.x -.5) * (glm::cos(60) / .5);

			dir.y = glm::sqrt(1 - pow(dir.x, 2));

			//pos = { 0,0 };
		}
		return;
	}

	dir /= (glm::length(dir) == 0) ? 1 : glm::length(dir);
	//pos += dir * speed * dt;

	glm::vec2 dir_speed = dir * speed * dt;
	glm::vec2 next_pos = pos + dir_speed + size/glm::vec2(2);

	const sque_vec<Entity*>& entities = EntitiesGet();
	for (uint32_t i = 0; i < entities.size(); ++i)
	{
		if (i == 1)
			continue;

		glm::vec2 ld(entities[i]->pos.x, entities[i]->pos.y);
		glm::vec2 ru(entities[i]->pos.x + entities[i]->size.x, entities[i]->pos.y + entities[i]->size.y);

		glm::vec2 near_v;
		near_v.x = glm::max(ld.x, glm::min(ru.x, next_pos.x));
		near_v.y = glm::max(ld.y, glm::min(ru.y, next_pos.y));
		glm::vec2 ray = near_v - next_pos ;
		float overlap = size.x/2. - glm::length(ray);
		if (overlap > 0)
		{
			next_pos -= glm::normalize(ray) * overlap * glm::vec2(2);
			if (abs(ray.y) > abs(ray.x))
				dir.y *= -1;
			else
				dir.x *= -1;

			entities[i]->OnCollision(&dir, &ball_state, pos);

			speed *= glm::length(dir);
			glm::normalize(dir);
		}		
	}
	
	pos = next_pos - size / glm::vec2(2);

	// Debug move
	dir.x = -(SQUE_INPUT_GetKey(SQUE_KEY_ARROW_LEFT) > 0 ) + (SQUE_INPUT_GetKey(SQUE_KEY_ARROW_RIGHT) > 0);
	dir.y = (SQUE_INPUT_GetKey(SQUE_KEY_ARROW_UP) > 0) + -(SQUE_INPUT_GetKey(SQUE_KEY_ARROW_DOWN) > 0);
}

void Ball::CleanUp()
{

}