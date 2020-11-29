#ifndef _LogHandler_H_
#define _LogHandler_H_

#include <cstdarg>

namespace FLYLOGGER
{
	#define LOGSIZE 512
	enum FlyLogType
	{
		LT_INFO = 4,
		LT_WARNING,
		LT_ERROR,
		LT_CRITICAL
		
	};
	struct FlyLog
	{
		int type = -1;
		FlyLogType lt = LT_INFO;
		char log[LOGSIZE] = {0};
	};
	void FlyPrintLog (const char* log, int lt);
	void DumpData();
	void INIT(bool dumpdata);
	void CLOSE();
	void LOG(FlyLogType lt, const char file[], int line, const char* format, ...);
	#define FLYLOG(LogType,format,...) LOG(LogType,__FILE__,__LINE__, format, ##__VA_ARGS__)
}
#endif
