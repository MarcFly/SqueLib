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
#elif defined(ANDROID)
#endif

// Return a const char* to a char* works, but does generate memory leak if untreated
// Should I make my own easy string to return these types?
// Should I use std::string?
std::string FLYFS_GetExecPath()
{
	char path[256];
	int len = 0;
#if defined(_WIN32)
	len = GetModuleFileNameA(NULL, path, 256);
#elif defined(__linux__)
#elif defined(ANDROID)
#endif
	return std::string(path);
}

bool FLYFS_CreateDirFullPath(const char* path)
{
	bool ret = true;
#if defined(_WIN32)
	ret = CreateDirectoryA(path, NULL);
#elif defined(__linux__)
#elif defined(ANDROID)
#endif
	return ret;
}

// Path should be written without anything before, just if you want to do things out of directory
bool FLYFS_CreateDirRelative(const char* path, int32_t flags)
{
	bool ret = true;
	std::string exec_path = FLYFS_GetExecPath();
	std::string exe = strrchr(exec_path.c_str(), FE);
	size_t v = exec_path.find(std::string_view(exe));
	exec_path = exec_path.substr(0, exec_path.length() - exe.length()) + FOLDER_ENDING + path;

	FLYFS_CreateDirFullPath(exec_path.c_str());

#if defined(_WIN32)
	SetFileAttributes(exec_path.c_str(), flags);
#elif defined(__linux__)
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
	in.read(data, len);
	in.close();

	return data;
}

bool FLYFS_WriteFileRaw(const char* path, byte* data)
{
	bool ret = true;

	return ret;
}