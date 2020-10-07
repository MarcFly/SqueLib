#ifndef _SIMPLE_PROFILER_
#define _SIMPLE_PROFILER_

#include<map>
#include<string>
#include<vector>
#include "LWTimer.h"

struct Scope
{
	Scope() {};
	int temp_id = -1;
	std::string custom_id = "Unnamed";
	std::vector<std::pair<int, unsigned int>> framedata; //in microsecs
	std::map<int, Scope*> subscopes;
	Scope* prev = nullptr;
	LW_Timer timer;
};

// Define Variables
static std::map<std::string, int> SCOPES_INPUT;
static std::map<int, std::string*> SCOPES_READ;

static std::vector<Scope*> scopes;
static std::vector<LW_Timer> active_timers;

static Scope* LastScope = nullptr;

static bool active = false;

// Define Functions

namespace SProf
{
	void Profiler_Close();

	void Profiler_Init(bool init);

	bool GetActive();

	void PUSH_SCOPE(const char file[], int line);

	void CLOSE_SCOPE();

	void ProfilerActive(bool start_active);


	#define INSCOPE_TIMER() PUSH_SCOPE(__FILE__,__LINE__)
	#define OUTSCOPE_TIMER() CLOSE_SCOPE();
	#define SProfiler_INIT(init) Profiler_Init(init) // Bool to start gathering data on init or when you later want
	#define Sprofiler_SwapActive() ProfilerActive(!SP.GetActive())
};

#endif