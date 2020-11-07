#include "Application.h"
#include "EngineCode/modules/Module.h"

#include "EngineCode/modules/ModuleTime.h"

Application::Application()
{
	time = new ModuleTime();
	modules.push_back(time);
}

Application::~Application()
{

}

bool Application::Init()
{
	bool ret = true;

	for (uint16 i = 0; i < modules.size() && ret == true; ++i)
		ret = modules[i]->Init();

	return ret;
}

bool Application::Update()
{
	bool ret = true;

	for (uint16 i = 0; i < modules.size() && ret == true; ++i)
		ret = modules[i]->Update(engine_time, game_time);

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (uint16 i = 0; i < modules.size() && ret == true; ++i)
	{
		ret = modules[i]->CleanUp();
		delete modules[i];
	}

	return ret;
}