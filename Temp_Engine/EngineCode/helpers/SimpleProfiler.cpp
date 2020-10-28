#include "SimpleProfiler.h"
#include <mutex>
#include <atomic>
#include <cstring>

// Define Variables
namespace SP
{
	std::mutex _mtx;

	static std::map<std::string, int> SCOPES_INPUT;
	static std::map<int, std::string*> SCOPES_READ;

	static std::vector<Scope*> scopes;
	static std::vector<LW_Timer> active_timers;

	static std::vector<Scope*> open_scopes;

	static std::atomic<bool> active = false;
}

using namespace SP;

void SProf::CLOSE()
{
	std::unique_lock<std::mutex> _lk(_mtx);
	if (open_scopes.size() > 0)
		open_scopes.clear();

	for (int i = 0; i < scopes.size(); ++i)
		delete scopes[i];
	scopes.clear();


}

void SProf::INIT(bool init)
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
			Scope* new_scope = nullptr;
			std::string sttr(strrchr(file, '\\') + std::string(": ") + std::to_string(line));


			std::unique_lock<std::mutex> _lk(_mtx);
			Scope* last_scope = (open_scopes.size() > 0) ? open_scopes[open_scopes.size()-1] : nullptr;

			if (SCOPES_INPUT.count(sttr) == 0)
			{
				// Initialize Scope and add it to last one, if it does not exist
				new_scope = new Scope();

				new_scope->temp_id = SCOPES_INPUT.size();
				if (last_scope != nullptr) last_scope->subscopes.insert(std::pair<int, Scope*>(new_scope->temp_id, new_scope));
				new_scope->prev = last_scope;
				last_scope = new_scope;
				scopes.push_back(new_scope);
			}

			auto it_push = SCOPES_INPUT.insert(std::pair<std::string, int>(sttr, SCOPES_INPUT.size()));
			SCOPES_READ.insert(std::pair<int, std::string*>(it_push.first->second, (std::string*) & it_push.first->first));

			new_scope = scopes[it_push.first->second];
			new_scope->timer.Start();
		}
	}
}

void SProf::CLOSE_SCOPE()
{
	std::unique_lock<std::mutex> _lk(_mtx);
	Scope* last_scope = (open_scopes.size() > 0) ? open_scopes[open_scopes.size()-1] : nullptr;
	if (last_scope != nullptr)
	{
		last_scope->framedata.push_back(std::pair<int, unsigned int>(last_scope->framedata.size(), last_scope->timer.Read_uS()));
		last_scope = last_scope->prev;

		open_scopes.pop_back();
	}
}

void SProf::ProfilerActive(bool start_active)
{
	active = start_active;
}