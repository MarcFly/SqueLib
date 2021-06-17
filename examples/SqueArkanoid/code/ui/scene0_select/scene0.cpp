#include "scene0.h"

SelectScene::SelectScene()
{

}

SelectScene::~SelectScene()
{

}

void SelectScene::Update()
{
	if(ImGui::Button("Play Level 1"))
	{
		SetPlayScene();
		return;
	}
}