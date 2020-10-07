#ifndef _LW_TIMER_H_
#define _LW_TIMER_H_

#include <chrono>

class LW_Timer
{
public:		// Constructors - Destructors
	LW_Timer() : start_at(std::chrono::high_resolution_clock::now()), stop_at(start_at), is_stopped(false) {};
	~LW_Timer() {};


private:	// Private Vars
	std::chrono::time_point<std::chrono::high_resolution_clock> start_at;
	std::chrono::time_point<std::chrono::high_resolution_clock> stop_at;
	bool is_stopped;

protected:	// Protected Vars

public:		// Public Vars

public:		// CPP Defined Functions
	void Start();
	void Stop();

	int Read_uS();
	int Read_mS();

public:		// Header Defined Functions - <5 lines
	bool IsStopped() const {
		return is_stopped;
	}
};

#endif // _LW_TIMER_H_