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

void PlayScene::SetupDemo()
{
	int32_t wx, wy;
	SQUE_RENDER_GetFramebufferSize(&wx, &wy);
	uint16_t cap_top = wx / (wy / 20.);

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
}

void PlayScene::SetupSurvival()
{
	if (!survival) ToggleSurvival();
	survival = true;

	// Random 1 by 1 tiles
	int32_t wx, wy;
	SQUE_RENDER_GetFramebufferSize(&wx, &wy);
	uint16_t cap_top = wx / (wy / 20.);

	for (uint32_t i = 9; i < 18; ++i)
	{
		for (uint32_t j = 2; j < cap_top; ++j)
			if (SQUE_RNG(4) == 0)
				EntitiesAdd(new Block(1 + SQUE_RNG(4), glm::vec2(j, i)));
	}

}

void PlayScene::Setup(uint32_t level)
{
	surv_timer.Start();
	EntitiesClear(); // Clear just to make sure
	AddScore(-GetScore());
	ModifyLives(-GetLives() + 3);

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
	for (int32_t i = 0; i < cap_top + 1; ++i)
	{
		EntitiesAdd(new Block(0, glm::vec2(0, i)));
		EntitiesAdd(new Block(0, glm::vec2(cap_top + 1, i)));
	}
	for (int32_t i = 0; i < cap_top + 1; ++i)
		EntitiesAdd(new Block(0, glm::vec2(i, 20)));


	if (level == 0)
		SetupDemo();
	else
		SetupSurvival();


	// Background
	for (int32_t i = 0; i < cap_top + 1; ++i)
		for (int32_t j = 0; j < cap_top + 1; ++j)
			EntitiesAddBackground(new BgBlock(0, glm::vec2(j, i), 1));
}

void PlayScene::Update()
{
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1, 1, 1, .9f));
	ImVec2 wsize = ImGui::GetWindowSize();
	wsize.y /= 10.;
	ImGui::BeginChild("##Child Win", wsize);
	{
		ImGui::Text("SCORE: %u", GetScore()); // Get from player...

		ImGui::SameLine();
		ImVec2 size = ImGui::GetWindowSize();

		ImGui::SetCursorPosX(wsize.x / 3.);
		ImGui::Text("Time: %.00f s", surv_timer.ReadMilliSec() / 1000.);

		ImGui::SameLine();
		ImGui::SetCursorPosX(2 * wsize.x / 3.);
		ImGui::Text("Lives: %u", GetLives());
	}

	const sque_vec<Entity*>& entities = EntitiesGet();
	if (GetLives() == 0 || (!survival && ((Block*)entities[entities.size() - 1])->hp == 0) || ImGui::Button("BACK", wsize))
	{
		// Lose Screen
		EntitiesClear();
		SetSelectScene();
		if (survival == true)
		{
			float time = surv_timer.ReadMilliSec() / 1000.;
			if (time > high_time)
				high_time = time;
			surv_timer.Kill();
			ToggleSurvival();
		}
		survival = false;
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();

	if (survival && ((Block*)entities[entities.size() - 1])->hp == 0)
	{
		SetupSurvival();
	}
}

void PlayScene::CleanUp()
{

}