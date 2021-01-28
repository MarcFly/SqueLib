// When using a part of the library as standalone, only include the standalone header

#ifdef LOGGER_SOLO
#   include "sque_logger.h"
#else
#   include "squelib.h"
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VARIABLE DEFINITION ///////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <mutex>
#include <unordered_map>
#include <vector>

typedef std::pair<int, SQUE_Log> PairLOG;
static std::mutex _mtx;
static std::unordered_map<std::string, int> Push_LogKeys;
static std::unordered_map<int, std::string*> Get_LogKeys;
static std::vector<PairLOG> logs;
static bool DUMPDATA = true;

#include <ctime>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <cstring>

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PLATFORM SPECIFICS ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////



#ifndef ANDROID
#   include <filesystem> // Will probably be superseeded by squelib import/export/read/write
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION AND STATE MANAGEMENT ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool SQUE_LOGGER_Init(bool dumpdata)
{
    bool ret = true;

    DUMPDATA = dumpdata;

    return ret;
}

void SQUE_LOGGER_Close()
{
    if (DUMPDATA) SQUE_LOGGER_DumpData();

    Push_LogKeys.clear();
    Get_LogKeys.clear();
    logs.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTION USAGE ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_LOGGER_Log(SQUE_LogType lt, const char file[], int line, const char* format, ...)
{
    std::lock_guard<std::mutex> lk(_mtx);

    SQUE_Log push;
    push.lt = lt;
    
    std::string sttr(strrchr(file, FOLDER_ENDING));

    auto it_push = Push_LogKeys.insert(std::pair<std::string, int>(sttr, Push_LogKeys.size()));
    Get_LogKeys.insert(std::pair<int, std::string*>(it_push.first->second, (std::string*)&it_push.first->first));
    
    push.type = Push_LogKeys.at(sttr);

    static va_list ap;

    int sizeminus = sttr.length() + 2 + 4;
    int calc_logsize = LOGSIZE - sizeminus;
    char* tmp = new char[calc_logsize];
    va_start(ap, format);
    int len = vsnprintf(tmp, calc_logsize, format, ap);
    va_end(ap);

    // For now let's just not take care of logs with bigger size, i think it crashes
    if(len > (calc_logsize)) 
    {
        SQUE_PrintVargs(lt, file, line, tmp);
        delete tmp;
        return;
    }
    sprintf(push.log, "%s(%d): %s", sttr.c_str(), line, tmp);

    logs.push_back(PairLOG(push.type, push));

    SQUE_ConsolePrint((int) lt, &push.log[0]);	
}

void SQUE_LOGGER_DumpData()
{
    // Replace by how asset manager does it
#ifndef ANDROID
    std::filesystem::create_directory("Logs");
#else

#endif
    std::time_t current = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::string filename("./Logs/");
    filename += std::ctime(&current);
    filename = filename.substr(0, filename.length() - 1);
    filename += ".log";

    std::replace(filename.begin(), filename.end(), ':', '-');

    std::fstream write_file;
    write_file.open(filename.c_str(), std::fstream::out | std::fstream::binary);


    int buf_size = (LOGSIZE + 1) * logs.size();
    char* data = new char[buf_size];
    char* cursor = data;

    std::string header("");
    for (int i = 0; i < logs.size(); ++i)
    {
        memcpy(cursor, logs[i].second.log, LOGSIZE + 1);
        cursor += LOGSIZE;
        memcpy(cursor, "\n", 1);
        cursor += 1;
    }

    write_file.write(data, buf_size);
    write_file.close();

    delete[] data;
}