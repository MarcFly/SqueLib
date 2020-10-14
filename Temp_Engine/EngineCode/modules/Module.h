#ifndef _Module_H_
#define _Module_H_

#include <vector>

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

public:
	const char* name = "";

private:
	std::vector<EVENT_TYPES> subscribed;
	
};

#endif // _Module_H_

