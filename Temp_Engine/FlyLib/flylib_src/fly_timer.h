#ifndef _FlyTimer_H_
#define _FlyTimer_H_

#include <stdint.h>
#include <atomic>

struct FlyTimer
{
	FlyTimer();
	~FlyTimer();

	void Start();
	void Stop();
	bool IsStopped() const;
	uint16_t ReadMilliSec() const;
	uint32_t ReadMicroSec() const;
	uint32_t ReadNanoSec() const;
	
private:
	uint32_t start_at_ns;
	uint32_t start_at_us;
	uint16_t start_at_ms;

	uint32_t stop_at_ns;
	uint32_t stop_at_us;
	uint16_t stop_at_ms;

	std::atomic<bool> is_stopped;
};

#endif // _FlyTime_H_