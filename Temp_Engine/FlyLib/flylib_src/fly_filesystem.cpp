#ifdef FILESYSTEM_SOLO
#	include "fly_filesystem.h"
#else
#	include "fly_lib.h"
#endif

// for now I will be using std::string, because its practical
#include <cstring>
#include <string>
#include <fstream>

// filesystem access is purely OS based i believe, so I will use the _WIN32, __linux__ and ANDROID defines
// If more platforms were to be added, i will do so

// for copy paste purposes
#if defined(_WIN32)
#elif defined(__linux__)
#elif defined(ANDROID)
#endif
//

#if defined(_WIN32)
#include <Windows.h>
#include <libloaderapi.h>
#elif defined(__linux__)
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
#elif defined(ANDROID)
#endif

// Return a const char* to a char* works, but does generate memory leak if untreated
// Should I make my own easy string to return these types?
// Should I use std::string?
std::string FLYFS_GetExecPath()
{
	std::string ret;
	char path[256];
	int len = 0;
#if defined(_WIN32)
	len = GetModuleFileNameA(NULL, path, 256);
	std::string exec_path(path);
	std::string exe = strrchr(path, FE);
	size_t v = exec_path.find(std::string_view(exe));
	exec_path = exec_path.substr(0, exec_path.length() - exe.length()) + FOLDER_ENDING + path;

#elif defined(__linux__)
	int32_t pid = getpid();
	char string[50];
	sprintf(string, "/proc/self/exe", pid);
	len = readlink("/proc/self/exe", path, 256);
	ret = dirname(path);
#elif defined(ANDROID)
#endif
	return ret;
}

bool FLYFS_CreateDirFullPath(const char* path)
{
	bool ret = true;
#if defined(_WIN32)
	ret = CreateDirectoryA(path, NULL);
#elif defined(__linux__)
	ret = (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) > 0);
#elif defined(ANDROID)
#endif
	return ret;
}

// Path should be written without anything before, just if you want to do things out of directory
bool FLYFS_CreateDirRelative(const char* path, int32_t flags)
{
	bool ret = true;
	std::string exec_path = FLYFS_GetExecPath() + FOLDER_ENDING + path;
	

	FLYFS_CreateDirFullPath(exec_path.c_str());

#if defined(_WIN32)
	SetFileAttributes(exec_path.c_str(), flags);
#elif defined(__linux__)
	if(CHK_FLAG(flags, FLYFS_HIDDEN))
	{
		int32_t v = exec_path.find(strrchr(exec_path.c_str(), FOLDER_ENDING));
		std::string old = exec_path;
		exec_path.insert(v+1, 1, '.');
		rename(old.c_str(), exec_path.c_str());
	}
#elif defined(ANDROID)
#endif

	return ret;
}

byte* FLYFS_LoadFileRaw(const char* file)
{
	byte* data = NULL;
	std::ifstream in;
	in.open(file, std::ios::binary | std::ios::in);
	in.seekg(0, std::ios::end);
	int len = in.tellg();
	in.seekg(0, std::ios::beg);
	data = new byte[len];
	in.read((char*)data, len);
	in.close();

	return data;
}

bool FLYFS_WriteFileRaw(const char* path, byte* data)
{
	bool ret = true;

	return ret;
}