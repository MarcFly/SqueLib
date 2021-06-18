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
	// Do the setup of a level
	// Create a player and ball
	// Load all the Blocks where they should
	// Player will always be 0 and Ball 1 in vector position (for ease of use)
	EntitiesAdd(new Player());
	EntitiesAdd(new Ball());

	EntitiesAdd(new Block(2, glm::vec2(0, 0)));
	EntitiesAdd(new Block(1, glm::vec2(0, 0)));
}

void PlayScene::Update()
{
	ImGui::Text("SCORE: %dpts", 100); // Get from player...

	ImGui::SameLine();
	ImVec2 size = ImGui::GetWindowSize();
	float x = ImGui::GetCursorPosX();

	ImGui::SetCursorPosX(size.x - size.x/4);
	ImGui::PushItemWidth(size.x / 4);
	if (ImGui::Button("BACK"))
		SetSelectScene();
	ImGui::PopItemWidth();

	ImGui::SetCursorPosX(x);
}

void PlayScene::CleanUp()
{

}