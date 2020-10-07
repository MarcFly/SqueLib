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
	if (is_stopped)
		return std::chrono::duration_cast<std::chrono::microseconds>(stop_at - start_at).count();
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_at).count();
}
int LW_Timer::Read_mS()
{
	if (is_stopped)
		return std::chrono::duration_cast<std::chrono::milliseconds>(stop_at - start_at).count();
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_at).count();
}