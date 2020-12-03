// When using a part of the library as standalone, only include the standalone header

#ifdef LOGGER_SOLO
#   include "fly_logger.h"
#else
#   include "fly_lib.h"
#endif



// Folder Ending On Windows can be '/' but it is not recognized that way using FS functions
#ifdef _WIN32
#   include <Windows.h>
#   define FOLDER_ENDING '\\'
#else
#   define FOLDER_ENDING '/'
#   ifdef ANDROID
#       include<android/log.h>
#   endif
#endif

// Var Define

#include <mutex>
#include <unordered_map>
#include <vector>

typedef std::pair<int, FlyLog> PairLOG;
static std::mutex _mtx;
static std::unordered_map<std::string, int> Push_LogKeys;
static std::unordered_map<int, std::string*> Get_LogKeys;
static std::vector<PairLOG> logs;
static bool DUMPDATA = true;

// Function Definitions


#include <ctime>
#include <chrono>
#include <fstream>
#include <algorithm>
#include <cstring>


void FlyPrintLog (const char* log, int lt)
{
#ifdef _WIN32
    OutputDebugString(log);
    OutputDebugString("\n");
#elif ANDROID
    __android_log_print(lt, "TempEngine", log);
#elif LINUX

#endif
}

#ifndef ANDROID
#   include <filesystem>
#endif

void DumpData()
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

bool FLYLOGGER_INIT(bool dumpdata)
{
    bool ret = true;

    DUMPDATA = dumpdata; 

    return ret;
}

void FLYLOGGER_CLOSE()
{
    if(DUMPDATA) DumpData();

    Push_LogKeys.clear();
    Get_LogKeys.clear();
    logs.clear();
}


// Log Output Functions
#ifdef ANDROID
#   include <android/log.h>
#elif _WIN32
//#   include <Windows.h>
#endif

void FLYLOGGER_LOG(FlyLogType lt, const char file[], int line, const char* format, ...)
{
    std::lock_guard<std::mutex> lk(_mtx);

    FlyLog push;
    push.lt = lt;
    
    std::string sttr(strrchr(file, FOLDER_ENDING));

    auto it_push = Push_LogKeys.insert(std::pair<std::string, int>(sttr, Push_LogKeys.size()));
    Get_LogKeys.insert(std::pair<int, std::string*>(it_push.first->second, (std::string*)&it_push.first->first));
    
    push.type = Push_LogKeys.at(sttr);

    static va_list ap;

    char tmp[LOGSIZE];
    va_start(ap, format);
    vsnprintf(tmp, LOGSIZE, format, ap);
    va_end(ap);

    sprintf(push.log, "%s(%d): %s", file, line, tmp);

    logs.push_back(PairLOG(push.type, push));

    FlyPrintLog(&push.log[0], (int)lt);	
}