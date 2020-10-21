#include <cstdarg>
#include <ctime>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <mutex>

#include <cstring>
#include <filesystem>

#include "LogHandler.h"

#include <iostream>

#ifdef _WIN32
#include <Windows.h>
#elif __linux__
#endif // OS Check

// Var Define
namespace LH
{
	static std::mutex _mtx;
	static std::unordered_map<std::string, int> Push_LogKeys;
	static std::unordered_map<int, std::string*> Get_LogKeys;
	static std::vector<PairLOG> logs;
	static bool DUMPDATA = true;
}

using namespace LH;

void LOGGER::INIT()
{

}

void LOGGER::CLOSE(bool dumpdata)
{
	DumpData();

	Push_LogKeys.clear();
	Get_LogKeys.clear();
	logs.clear();
}

void LOGGER::LOG(LogType lt, const char file[], int line, const char* format, ...)
{
	std::lock_guard<std::mutex> lk(_mtx);

	Log push;
	push.lt = lt;
	const char* test = strrchr(file, '\\');
	std::cout << test << std::endl;
	std::string sttr;
	if(test != nullptr)
		sttr = std::string(test);
	(strrchr(file, '\\'));
	auto it_push = Push_LogKeys.insert(std::pair<std::string, int>(sttr, Push_LogKeys.size()));
	Get_LogKeys.insert(std::pair<int, std::string*>(it_push.first->second, (std::string*)&it_push.first->first));
	
	push.type = Push_LogKeys.at(sttr);

	static va_list ap;

	char tmp[LOGSIZE-6];
	va_start(ap, format);
	vsnprintf(tmp, LOGSIZE, format, ap);
	va_end(ap);

	sprintf(push.log, "%s(%d): %s", file, line, tmp);

	logs.push_back(PairLOG(push.type, push));

	std::cout << push.log << std::endl;

#ifdef _WIN32
	OutputDebugString(push.log);
	OutputDebugString("\n");
#endif
}

void LOGGER::DumpData()
{
	std::filesystem::create_directory("Logs");

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

	std::string header("");
	for (int i = 0; i < logs.size(); ++i)
	{
		memcpy(cursor, logs[i].second.log, LOGSIZE);
		cursor += LOGSIZE;
	}

	write_file.write(data, buf_size);
	write_file.close();

	delete[] data;
	
}