#ifndef _Module_H_
#define _Module_H_

#include <vector>
#include <atomic>
#include <mutex>

enum EVENT_TYPES {
	
	ET_ERROR
};

class Module {

public:
	Module() {};
	~Module() {};

	virtual bool Init() { return true; };
	virtual bool Update(float engine_dt, float game_dt) { return true; };
	virtual bool CleanUp() { return true; };

	virtual bool LoadConfig(/* Config Type file*/) { return true; };
	virtual bool SaveConfig(/* Config Type file*/) { return true; };

public:
	const char* name = "";

private:
	std::vector<EVENT_TYPES> subscribed;
	std::mutex _mtx;
	
};

#endif // _Module_H_

