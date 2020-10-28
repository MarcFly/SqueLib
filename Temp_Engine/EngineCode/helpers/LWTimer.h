#ifndef _LW_TIMER_H_
#define _LW_TIMER_H_

#include <chrono>
#include <atomic>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> high_res_clock;

class LW_Timer
{
public:		// Constructors - Destructors
	LW_Timer() : start_at(std::chrono::high_resolution_clock::now()), stop_at(start_at), is_stopped(false) {};
	~LW_Timer() {};


private:	// Private Vars
	high_res_clock start_at;
	high_res_clock stop_at;
	std::atomic<bool> is_stopped;

protected:	// Protected Vars

public:		// Public Vars

public:		// CPP Defined Functions
	void Start();
	void Stop();

	int Read_mS();
	int Read_uS();
	int Read_nS();

public:		// Header Defined Functions - <5 lines
	bool IsStopped() const {
		return is_stopped;
	}
};

#endif // _LW_TIMER_H_