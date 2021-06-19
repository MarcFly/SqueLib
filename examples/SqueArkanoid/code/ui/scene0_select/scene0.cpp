#include "scene0.h"

SelectScene::SelectScene()
{

}

SelectScene::~SelectScene()
{

}

void SelectScene::Update()
{

	ImVec2 size = ImGui::GetWindowSize();
	ImVec2 cursor = ImGui::GetCursorPos();
	ImVec2 c_cursor = cursor;
	cursor.x = size.x / 2. - size.x / 10.;
	cursor.y = size.y / 2. - size.y / 20.;
	//ImGui::SetCursorPos(cursor);
	if(ImGui::Button("Play Timed"))
	{
		SetPlayScene();
		return;
	}

	ImGui::SameLine();
	ImGui::Text("Top Score: %u", GetHiscore());

	if (ImGui::Button("Play Survival"))
	{
		SetPlayScene(1);
		return;
	}

	ImGui::SameLine();
	ImGui::Text("Top Score: %u in %.2f sec", GetSHiscore() , GetHiTime());
	ImGui::SameLine();


	ImGui::SetCursorPos(c_cursor);
}