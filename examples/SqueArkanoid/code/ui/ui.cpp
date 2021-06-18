#include "ui.h"
#include <squelib.h>
#include <imgui.h>
#include <imgui_impl_squelib.h>

// Include all ui items here...
#include "scene0_select/scene0.h"
#include "scene1_play/scene1.h"

sque_vec<UI_Item*> items;
// Each scene will be set to disabled when another is selected...
// Scenes should be -> Play(0), Select Level(1), Playing(2)

UI_Item::UI_Item()
{

}

UI_Item::~UI_Item()
{

}

void SetPlayScene(uint32_t level)
{
    for (uint32_t i = 0; i < items.size(); ++i)
        items[i]->active = false;
    items[1]->active = true;

    ((PlayScene*)items[1])->Setup(level);
}
void SetSelectScene()
{
    for (uint32_t i = 0; i < items.size(); ++i)
        items[i]->active = false;
    items[0]->active = true;
}
void UiInit()
{
    ImGui::CreateContext();
    ImGui::StyleColorsLight();
    ImGui_ImplSqueLib_Init();
    // Scale ui appropiately...
    ImGui::GetIO().FontGlobalScale *= SQUE_DISPLAY_GetDPIDensity() / 72;
    ImGui::GetStyle().ScaleAllSizes(SQUE_DISPLAY_GetDPIDensity() / 100);
    items.push_back(new SelectScene());
    items.push_back(new PlayScene());
}

void UiPreUpdate()
{
    ImGui_ImplSqueLib_NewFrame();
    ImGui::NewFrame();

    // ImGui Begin an empty window in which to draw in...
    int32_t vpx, vpy;
    SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
    
    ImGui::SetNextWindowSize(ImVec2(vpx,vpy + 20));
    ImGui::SetNextWindowPos(ImVec2(0, -20));
    ImGui::Begin("UI_Window", NULL, (items[1]->active * ImGuiWindowFlags_NoBackground) | ImGuiWindowFlags_NoDecoration);

}

void UiRender()
{
    for (uint32_t i = 0; i < items.size(); ++i)
        if(items[i]->active)
            items[i]->Update();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplSqueLib_Render(ImGui::GetDrawData());
}

void UiCleanUp()
{
    for (uint32_t i = 0; i < items.size(); ++i)
    {
        items[i]->CleanUp();
        delete items[i];
    }
    items.clear();
}