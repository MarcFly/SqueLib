#include "killzone.h"

KillZone::KillZone()
{
	int32_t wx, wy;
	SQUE_DISPLAY_GetWindowSize(&wx, &wy);
	
	size = glm::vec2(wx, wy / 20.);
	pos = glm::vec2(0, -size.y);
}

KillZone::~KillZone()
{

}

#include "player.h"

void KillZone::OnCollision(glm::vec2* dir_speed, uint32_t* ball_state, const glm::vec2 c_pos)
{
	EntitiesGet()[1]->Init();
	Player* p = (Player*)EntitiesGet()[0];
	--p->hp;
}