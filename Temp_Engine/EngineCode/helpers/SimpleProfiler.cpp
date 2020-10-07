#include "SimpleProfiler.h"

void SProf::Profiler_Close()
{
	if (LastScope != nullptr)
		LastScope = nullptr;

	for (int i = 0; i < scopes.size(); ++i)
		delete scopes[i];
	scopes.clear();


}

void SProf::Profiler_Init(bool init)
{
	active = init;
}

bool SProf::GetActive()
{
	return active;
}

void SProf::PUSH_SCOPE(const char file[], int line)
{
	{
		if (GetActive())
		{
			Scope* newscope = nullptr;
			std::string sttr(strrchr(file, '\\') + std::string(": ") + std::to_string(line));


			if (SCOPES_INPUT.count(sttr) == 0)
			{
				// Initialize Scope and add it to last one, if it does not exist
				newscope = new Scope();

				newscope->temp_id = SCOPES_INPUT.size();
				if (LastScope != nullptr) LastScope->subscopes.insert(std::pair<int, Scope*>(newscope->temp_id, newscope));
				newscope->prev = LastScope;
				LastScope = newscope;
				scopes.push_back(newscope);
			}

			auto it_push = SCOPES_INPUT.insert(std::pair<std::string, int>(sttr, SCOPES_INPUT.size()));
			SCOPES_READ.insert(std::pair<int, std::string*>(it_push.first->second, (std::string*) & it_push.first->first));

			newscope = scopes[it_push.first->second];
			newscope->timer.Start();
		}
	}
}

void SProf::CLOSE_SCOPE()
{
	if (LastScope != nullptr)
	{
		LastScope->framedata.push_back(std::pair<int, unsigned int>(LastScope->framedata.size(), LastScope->timer.Read_uS()));
		LastScope = LastScope->prev;
	}
}

void SProf::ProfilerActive(bool start_active)
{
	active = start_active;
}