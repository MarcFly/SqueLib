#ifdef FILESYSTEM_SOLO
#	include "squefilesystem.h"
#else
#	include "squelib.h"
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
const char* SQUE_FS_GetExecPath()
{
	char* ret = NULL;
	char* path = new char[256];
	path = NULL;
	int len = 0;
#if defined(_WIN32)
	len = GetModuleFileNameA(NULL, path, 256);
	std::string exec_path(path);
	std::string exe = strrchr(path, FE);
	exec_path = exec_path.substr(0, exec_path.length() - exe.length());
	ret = new char[exec_path.length()+1];
	memcpy(ret, exec_path.c_str(), exec_path.length()+1);
	delete path;
#elif defined(ANDROID)
#elif defined(__linux__)
	int32_t pid = getpid();
	char string[50];
	sprintf(string, "/proc/self/exe/%d\0", pid);
	len = readlink("/proc/self/exe", path, 256);
	ret = dirname(path);
	//delete path;
#endif

	return ret;
}

bool SQUE_FS_CreateDirFullPath(const char* path)
{
	bool ret = SQUE_AskPermissions("WRITE_EXTERNAL_STORAGE") && SQUE_AskPermissions("WRITE_MEDIA_STORAGE");
	SQUE_PRINT(LT_INFO, "%d %d", SQUE_AskPermissions("WRITE_EXTERNAL_STORAGE"), SQUE_AskPermissions("WRITE_MEDIA_STORAGE"));
#if defined(_WIN32)
	ret = CreateDirectoryA(path, NULL);
#elif defined(ANDROID)
#elif defined(__linux__)
	ret = (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) > 0);
#endif
	return ret;
}

// Path should be written without anything before, just if you want to do things out of directory
bool SQUE_FS_CreateDirRelative(const char* path, int32_t flags)
{
	bool ret = true;
	std::string exec_path = std::string(SQUE_FS_GetExecPath()) + FOLDER_ENDING + path;
	

	SQUE_FS_CreateDirFullPath(exec_path.c_str());

#if defined(_WIN32)
	SetFileAttributes(exec_path.c_str(), flags);
#elif defined(ANDROID)

#elif defined(__linux__)
	if(CHK_FLAG(flags, SQUE_FS_HIDDEN))
	{
		int32_t v = exec_path.find(strrchr(exec_path.c_str(), FOLDER_ENDING));
		std::string old = exec_path;
		exec_path.insert(v+1, 1, '.');
		rename(old.c_str(), exec_path.c_str());
	}

#endif

	return ret;
}

SQUE_Asset* SQUE_FS_LoadFileRaw(const char* file)
{
	
	char* data = NULL;
	std::ifstream in;
	in.open(file, std::ios::binary | std::ios::in);
	if(!in) return NULL;
	in.seekg(0, std::ios::end);
	int len = in.tellg();
	in.seekg(0, std::ios::beg);
	data = new char[len];
	in.read((char*)data, len);
	in.close();

	SQUE_Asset* ret = new SQUE_Asset();
	ret->size = len;
	ret->raw_data = data;
	
	return ret;
}

bool SQUE_FS_WriteFileRaw(const char* path, char* data)
{
	bool ret = true;

	return ret;
}

// RAW ASSET READING / LOADING

SQUE_Dir* SQUE_FS_CreateBaseDirTree()
{
	SQUE_Dir* ret = new SQUE_Dir();
#if defined(ANDROID)
	memcpy(ret->name, "raw", 4);
	//ret->name = "raw";
	ret->native_dir_data = (char*)AAssetManager_openDir(my_app->activity->assetManager, ret->name);
	int dirs = 1;
	while (dirs)
	{
		const char* name = AAssetDir_getNextFileName((AAssetDir*)ret->native_dir_data);
		SQUE_PRINT(LT_INFO, " %s", name);
		if (name == NULL)
			break;
		else
		{
			AAssetDir* dir = AAssetManager_openDir(my_app->activity->assetManager, name);
			if (dir != NULL)
			{
				SQUE_Dir* branch = new SQUE_Dir();
				memcpy(branch->name, name, strlen(name));
				//branch->name = name;
				branch->native_dir_data = (char*)branch;
				branch->parent = ret;
				ret->children.push_back(branch);
			}
			else
				continue;
		}
	}
#else
	std::string main_path = SQUE_FS_GetExecPath();
	memcpy(ret->name, main_path.c_str(), main_path.length() + 1);

#endif
	return ret;
}

SQUE_Asset* SQUE_FS_LoadAssetRaw(const char* file)
{
	SQUE_Asset* ret = NULL;
#if defined(ANDROID)
	ret = new SQUE_Asset();
	AAsset* asset = AAssetManager_open(my_app->activity->assetManager, file, AASSET_MODE_BUFFER);
	SQUE_PRINT(LT_INFO, "%d %s", asset, file);
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
	char* sprint_v = new char[256];
	int len = sprintf(sprint_v, "%s/Assets/%s", SQUE_FS_GetExecPath(), file);
	if (len > 256)
	{
		delete sprint_v;
		sprint_v = new char[len];
		sprintf(sprint_v, "%s/Assets/%s", SQUE_FS_GetExecPath(), file);
	}
	ret = SQUE_FS_LoadFileRaw(sprint_v);
	delete sprint_v;
#endif

	return ret;
}