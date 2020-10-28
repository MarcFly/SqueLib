#include "LWTimer.h"

void LW_Timer::Start() {
	start_at = std::chrono::high_resolution_clock::now();
	stop_at = start_at;
	is_stopped = false;
}
void LW_Timer::Stop()
{
	is_stopped = true;
}
int LW_Timer::Read_uS()
{
	high_res_clock now = std::chrono::high_resolution_clock::now();
	if (is_stopped)
		return std::chrono::duration_cast<std::chrono::microseconds>(stop_at - start_at).count();
	return std::chrono::duration_cast<std::chrono::microseconds>(now - start_at).count();
}
int LW_Timer::Read_mS()
{
	high_res_clock now = std::chrono::high_resolution_clock::now();
	if (is_stopped)
		return std::chrono::duration_cast<std::chrono::milliseconds>(stop_at - start_at).count();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now - start_at).count();
}

int LW_Timer::Read_nS()
{
	high_res_clock now = std::chrono::high_resolution_clock::now();
	if (is_stopped)
		return std::chrono::duration_cast<std::chrono::nanoseconds>(stop_at - start_at).count();
	return std::chrono::duration_cast<std::chrono::nanoseconds>(now - start_at).count();
}