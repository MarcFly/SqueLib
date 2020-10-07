#include <cstdarg>
#include <ctime>
#include <chrono>
#include <fstream>
#include <algorithm>

#include "LogHandler.h"

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#endif // OS Check

void LOGGER::LH_INIT()
{
}

void LOGGER::LH_CLOSE(bool dumpdata)
{
	DumpData();

	Push_LogKeys.clear();
	Get_LogKeys.clear();
}

void LOGGER::LOG(LogType lt, const char file[], int line, const char* format, ...)
{
	Log push;
	push.lt = lt;
	std::string sttr(strrchr(file, '\\'));
	auto it_push = Push_LogKeys.insert(std::pair<std::string, int>(sttr, Push_LogKeys.size()));
	Get_LogKeys.insert(std::pair<int, std::string*>(it_push.first->second, (std::string*)&it_push.first->first));
	
	push.type = Push_LogKeys.at(sttr);

	static va_list ap;

	char tmp[LOGSIZE];
	va_start(ap, format);
	vsprintf_s(tmp, LOGSIZE, format, ap);
	va_end(ap);

	sprintf(push.log, "%s(%d): %s", file, line, tmp);

	logs.push_back(PairLOG(push.type, push));

#ifdef _WIN32
	OutputDebugString(push.log);
#elif __linux__
#endif
}

void LOGGER::DumpData()
{

	CreateDirectory("Logs", NULL);

	std::time_t current = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	std::string filename("./Logs/");
	filename += std::ctime(&current);
	filename = filename.substr(0, filename.length() - 1);
	filename += ".log";

	std::replace(filename.begin(), filename.end(), ':', '-');

	std::fstream write_file;
	write_file.open(filename.c_str(), std::fstream::out | std::fstream::binary);


	int buf_size = (LOGSIZE)* logs.size();
	char* data = new char[buf_size];
	char* cursor = data;

	std::string header;
	for (int i = 0; i < logs.size(); ++i)
	{
		memcpy(cursor, logs[i].second.log, LOGSIZE);
		cursor += LOGSIZE;
	}

	write_file.write(data, buf_size);
	write_file.close();

	delete[] data;
	
}