#ifdef FILESYSTEM_SOLO
#	include "squefilesystem.h"
#else
#	include "squelib.h"
#endif

// for now I will be using std::string, because its practical

//#include <fstream>


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

static char exec_path[512];

void SQUE_FS_Init()
{
	int len = 0;
#if defined(_WIN32)
	len = GetModuleFileNameA(NULL, exec_path, 512);
	char* exe = strrchr(exec_path, FE);
	exec_path[len-strlen(exe)] = '\0';
#elif defined(ANDROID)
#elif defined(__linux__)
	char link_temp[512] = "\0";
	len = readlink("/proc/self/exe", link_temp, 512);
	const char* dir = dirname(link_temp);
	sprintf(exec_path, "%s", dir);
#endif
}

const char* SQUE_FS_GetExecPath()
{
	return exec_path;
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
	data = new char[len+1];
	in.read((char*)data, len);
	in.close();

	SQUE_Asset* ret = new SQUE_Asset();
	ret->size = len+1;
	ret->raw_data = data;
	memcpy(&ret->raw_data[len], "\0", 1);
	
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
		char* tmp = new char[ret->size+1];
		memcpy(tmp, AAsset_getBuffer(asset), ret->size);
		tmp[ret->size] = '\0';
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
	int len = sprintf(sprint_v, "%s/Assets/%s\0", SQUE_FS_GetExecPath(), file);
	if (len > 256)
	{
		delete sprint_v;
		sprint_v = new char[len];
		sprintf(sprint_v, "%s/Assets/%s\0", SQUE_FS_GetExecPath(), file);
	}
	ret = SQUE_FS_LoadFileRaw(sprint_v);
	delete sprint_v;
#endif

	return ret;
}

const char* SQUE_FS_GetFileName(const char* file)
{
	const char* ret1 = strrchr(file, '/');
	if (ret1 != NULL)
		return ret1;
	const char* ret2 = strrchr(file, '\\');
	if (ret2 != NULL)
		return ret2;
	return file;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FILEWATCHER //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <filesystem>

enum SQUE_FS_Status
{
	SQFS_UNKNOW = 0,
	SQFS_NEW,
	SQFS_CHANGED,
	SQFS_DELETED,
};

SQUE_Dir SQUE_FS_GenDir(const char* location, const uint32_t parent_id)
{
	SQUE_Dir d;
	d.id = SQUE_RNG();
	const char* tmp = strrchr(location, '.');
	if (tmp != NULL)
	{
		uint32_t l = SQUE_FS_GetParentDir(location);
		l = (l < sizeof(d.location)) ? l : sizeof(d.location);
		memcpy(d.location, location, l);
	}
	else
		memcpy(d.location, location, sizeof(d.location));

	memcpy(d.name, SQUE_FS_GetFileName(d.location), sizeof(d.name));

	return d;
}

uint32_t SQUE_FS_GetParentDir(const char* path)
{
	uint32_t ret = strlen(path);
	const char* fw_slash = strrchr(path, '/');
	if(fw_slash != NULL)
		return ret -= strlen(fw_slash);

	const char* d_slash = strrchr(path, '\\');
	return ret -= (strlen(d_slash));
}

uint32_t AddToParent(const uint32_t id, const char* location, sque_vec<SQUE_Dir>& dirs)
{
	uint32_t len_dir = SQUE_FS_GetParentDir(location);
	char* tmp = new char[len_dir+1];
	memcpy(tmp, location, len_dir);
	tmp[len_dir] = '\0';
	for (uint16_t i = 0; i < dirs.size(); ++i)
	{
		if (strcmp(dirs[i].location, tmp) == 0)
		{
			dirs[i].children_ids.push_back(id);
			delete tmp;
			return dirs[i].id;
		}
	}
	delete tmp;
	return -1;
}

void SQUE_FS_GenDirectoryStructure(const char* location, sque_vec<SQUE_Dir>& dirs)
{
	dirs.push_back(SQUE_FS_GenDir(location));

	for (auto& file : std::filesystem::recursive_directory_iterator(dirs[0].location))
	{
		if (file.is_directory())
		{
			dirs.push_back(SQUE_FS_GenDir(file.path().string().c_str()));
			dirs.last()->parent_id = AddToParent(dirs.last()->id, dirs.last()->location, dirs);
		}
	}
}

sque_vec<char*> SQUE_FS_CheckDirectoryChanges(const char* path, sque_vec<SQUE_CtrlAsset*> assets_in_dir, HandleNewAssetLocation* handle_fun)
{
	for (uint32_t i = 0; i < assets_in_dir.size(); ++i)
	{
		assets_in_dir[i]->status = 0;
		if (!std::filesystem::exists(assets_in_dir[i]->location))
			assets_in_dir[i]->status = 2;
	}
	sque_vec<char*> new_items;
	for (auto& file : std::filesystem::recursive_directory_iterator(path))
	{
		// string hashing for ids... the MD5 old one might be great for that, it was decently fast
		uint32_t i;
		for (i = 0; i < assets_in_dir.size(); ++i)
		{
			if (assets_in_dir[i]->status == 2) continue;
			if (strcmp(assets_in_dir[i]->location, file.path().string().c_str()) == 0)
				break;
		}
		if (i < assets_in_dir.size())
			assets_in_dir[i]->status = 1;
		else
		{
			char* loc = new char[512];
			//char* t = file.path().string().c_str();
			memcpy(loc, file.path().string().c_str(), 512);
			new_items.push_back(loc);
		}
	}
	return new_items;
}