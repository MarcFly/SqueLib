#include "scene1.h"
#include "../../entities/types/include_all.h"

PlayScene::PlayScene()
{
	active = false;
}

PlayScene::~PlayScene()
{

}

void PlayScene::Init()
{
	// Will load here all scenes to play
	
	//EntitiesAdd(new Ball());

}

void PlayScene::Setup(uint32_t level)
{
	EntitiesClear(); // Clear just to make sure
	int32_t wx, wy;
	SQUE_RENDER_GetFramebufferSize(&wx, &wy);
	// Do the setup of a level
	// Create a player and ball
	// Load all the Blocks where they should
	// Player will always be 0 and Ball 1 in vector position (for ease of use)
	EntitiesAdd(new Player());
	EntitiesAdd(new Ball());

	EntitiesAdd(new KillZone());
	uint16_t cap_top = wx / (wy / 20.);
	for (int32_t i = 0; i < cap_top+1; ++i)
	{
		EntitiesAdd(new Block(0, glm::vec2(0, i)));
		EntitiesAdd(new Block(0, glm::vec2( cap_top +1, i)));
	}
	for (int32_t i = 0; i < cap_top+1; ++i)
		EntitiesAdd(new Block(0, glm::vec2(i, 20)));
	
	for (int32_t i = 17; i < 18; ++i)
	{
		for (int32_t j = 1; j < cap_top; j += 2)
			EntitiesAdd(new Block(4, glm::vec2(j, i), 2));
	}
	
	for (int32_t i = 14; i < 16; ++i)
	{
		for (int32_t j = 1; j < cap_top / 2; j += 1)
			EntitiesAdd(new Block(3, glm::vec2(j, i), 2));
	}
	
	for (int32_t i = 11; i < 13; ++i)
	{
		for (int32_t j = 1; j < cap_top; j += 2)
			EntitiesAdd(new Block(2, glm::vec2(j, i), 2));
	}
	
	for (int32_t i = 8; i < 10; ++i)
	{
		for (int32_t j = 1; j < cap_top; j += 2)
			EntitiesAdd(new Block(1, glm::vec2(j, i), 2));
	}

	// Background
	for (int32_t i = 0; i < cap_top + 1; ++i)
		for (int32_t j = 0; j < cap_top + 1; ++j)
			EntitiesAddBackground(new BgBlock(0, glm::vec2(j, i), 1));
}

void PlayScene::Update()
{

		ImGui::Text("SCORE: %dpts", 100); // Get from player...

		ImGui::SameLine();
		ImVec2 size = ImGui::GetWindowSize();
		float x = ImGui::GetCursorPosX();

		ImGui::SetCursorPosX(size.x - size.x / 4);
		ImGui::PushItemWidth(size.x / 4);
		if (ImGui::Button("BACK"))
			SetSelectScene();
		ImGui::PopItemWidth();

		ImGui::SetCursorPosX(x);


	if (((Player*)EntitiesGet()[0])->hp == 0)
	{
		// Lose Screen
		SetSelectScene();
	}
}

void PlayScene::CleanUp()
{

}