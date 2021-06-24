#ifndef _UI_H_
#define _UI_H_

#include <imgui.h>
#include "code/entities/entity.h"

void SetPlayScene(uint32_t level = 0);
void SetSelectScene();
//...

class UI_Item
{
public:
	UI_Item();
	~UI_Item();
	
	bool active = true;
	ImVec2 pos;
	ImVec2 size;
	uint32_t sound_handle;

	virtual void Init() {};
	virtual void Update() {};
	virtual void CleanUp() {};
};

void UiInit();
void UiPreUpdate();
void UiRender();
void UiCleanUp();

float GetHiTime();

#endif