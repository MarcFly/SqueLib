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
#elif defined(ANDROID)
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android_native_app_glue.h>
extern struct android_app* my_app;
#elif defined(__linux__)
#include <unistd.h>
#include <sys/stat.h>
#include <libgen.h>
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
#elif defined(ANDROID)
#elif defined(__linux__)
	int32_t pid = getpid();
	char string[50];
	sprintf(string, "/proc/self/exe", pid);
	len = readlink("/proc/self/exe", path, 256);
	ret = dirname(path);
#endif
	return ret;
}

bool FLYFS_CreateDirFullPath(const char* path)
{
	bool ret = true;
#if defined(_WIN32)
	ret = CreateDirectoryA(path, NULL);
#elif defined(ANDROID)
#elif defined(__linux__)
	ret = (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) > 0);
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
#elif defined(ANDROID)

#elif defined(__linux__)
	if(CHK_FLAG(flags, FLYFS_HIDDEN))
	{
		int32_t v = exec_path.find(strrchr(exec_path.c_str(), FOLDER_ENDING));
		std::string old = exec_path;
		exec_path.insert(v+1, 1, '.');
		rename(old.c_str(), exec_path.c_str());
	}

#endif

	return ret;
}

char* FLYFS_LoadFileRaw(const char* file)
{
	char* data = NULL;
	std::ifstream in;
	in.open(file, std::ios::binary | std::ios::in);
	in.seekg(0, std::ios::end);
	int len = in.tellg();
	in.seekg(0, std::ios::beg);
	data = new char[len];
	in.read((char*)data, len);
	in.close();

	return data;
}

bool FLYFS_WriteFileRaw(const char* path, char* data)
{
	bool ret = true;

	return ret;
}

// RAW ASSET READING / LOADING

FLY_Dir* FLYFS_CreateBaseDirTree()
{
	FLY_Dir* ret = new FLY_Dir();
#if defined(ANDROID)
	ret->name = "raw";
	ret->native_dir_data = (char*)AAssetManager_openDir(my_app->activity->assetManager, ret->name);
	int dirs = 1;
	while(dirs)
	{
		const char* name = AAssetDir_getNextFileName((AAssetDir*)ret->native_dir_data);
		FLYPRINT(LT_INFO, " %s", name);
		if(name == NULL)
			break;
		else
		{
			AAssetDir* dir = AAssetManager_openDir(my_app->activity->assetManager, name);
			if(dir != NULL)
			{
				FLY_Dir* branch = new FLY_Dir();
				branch->name = name;
				branch->native_dir_data = (char*)branch;
				branch->parent = ret;
				ret->children.push_back(branch);
			}
			else
				continue;
		}
	}
#else
	ret->name = FLYFS_GetExecPath().c_str();
#endif
	return ret;
}

void FLYFS_UpdateTree(FLY_Dir* root)
{

}

FLY_Dir* FLYFS_GetDirInTree(FLY_Dir* root, const char* leaf)
{

}

FLY_Asset* FLYFS_GetAssetRaw(FLY_Dir* start_dir, const char* file)
{
	FLY_Asset* ret = new FLY_Asset();
#if defined(ANDROID)
	AAsset* asset = AAssetManager_open(my_app->activity->assetManager, file, AASSET_MODE_BUFFER);
	FLYPRINT(LT_INFO, "%d %s", asset, file);
	if(asset != NULL)
	{
		ret->size = AAsset_getLength(asset);
		char* tmp = new char[ret->size];
		memcpy(tmp, AAsset_getBuffer(asset), ret->size);
		ret->raw_data = tmp;
		AAsset_close(asset);
	}
	else
	{
		delete ret;
		ret = NULL;
	}
#else
	// Here I should construct path from main path then load raw file
#endif
}