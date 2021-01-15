////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVAR SYSTEM /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// https://vkguide.dev/docs/extra-chapter/cvar_system/ /////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <string_utils.h>

enum class CVarFlags : uint32
{
	FLYCVAR_NONE = 0,
	FLYCVAR_NO_EDIT = BITSET1,
	FLYCVAR_EDIT_READ_ONLY = BITSET2,
	FLYCVAR_ADVANCED = BITSET3,

	FLYCVAR_EDIT_CHECKBOX = BITSET8,
	FLYCVAR_FLOAT_DRAG = BITSET9,

	FLYCVAR_MAX = BITSET32
}

// To Implement Auto Cvars, part of the engine not library
// I want to change from static array to a resizable array
// Back to learning c++ properly
template<typename T>
struct AutoCVar
{
protected:
	int32 index;
	using CVarType = T;
}

struct AutoCVar_Float : AutoCVar<float>
{
	AutoCVar_Float(const char* name, const char* description, float def_value, CVarFlags flags = CVarFlags)
	float Get();
	void Set(float val);
}

struct AutoCVar_Int : AutoCVar<int32>
{
	AutoCVar_Int(const char* name, const char* description, int def_value, CVarFlags flags = CVarFlags);

	int32 Get();
	void Set(int32 val);
}

struct AutoCVar_String : AutoCVar<std::string>
{
	AutoCVar_String(const char* name, const char* description, const char* def_value, CVarFlags flags = CVarFlags);

	const char* Get();
	void Set(const char* val);
}

class CVarSystem
{
// Constructors / Destructor


public:
	static CVarSystem* Get();
	
	// Getters
	virtual CVarParamete* GetCVar(StringUtils::StringHash hash) = 0;
	virtual float* GetFloatCVar(StringUtils::StringHash hash) = 0;
	virtual int32* GetIntCVar(StringUtils::StringHash hash) = 0;
	virtual const char* GetStringCVar(StringUtils::StringHash hash) = 0;

	// Setters
	virtual void SetFloatCVar(StringUtils::StringHash hash, float value) = 0;
	virtual void SetIntCVar(StringUtils::StringHash hash, int32 value) = 0;
	virtual void SetStringCVar(StringUtils::StringHash hash, const char* value) = 0;

	// Creators
	virtual CVarParameter* CreateFloatCVar(const char* name, const char* description, float def_value, float curr_value) = 0;
	virtual CVarParameter* CreateIntCVar(const char* name, const char* description, int def_value, int curr_value) = 0;
	virtual CVarParameter* CreateStringCVar(const char* name, const char* description, std::string def_value, std::string curr_value) = 0;

private:
	CVarParameter* InitCVar(const char* name, const char* description);
	std::unordered_map<uint32, CVarParameter> saved_cvars;
}	


