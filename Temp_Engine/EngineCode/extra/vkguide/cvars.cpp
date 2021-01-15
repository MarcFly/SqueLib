#ifdef CVARS_SOLO
#   include <fly_cvars.h>
#else
#   include <fly_lib.h>
#endif

#include <string>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CVAR TYPES //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum class CVarType : uchar
{
	INT,
	FLOAT,
	STRING
}

class CVarParameter
{
public:
	friend class CVarSystemImpl;
	
    int32 array_index;
    CVarType type;
    CVarFlags flags;
    std::string name;
    std::string description;
}

template<typename T>
struct CVarStorage
{
    T initial;
    T current;
    CVarParameter* parameter;
}

template<typename T>
struct CVarArray
{
    CVarStorage<T>* cvars;
    int32 last_cvar = 0;

    CVarArray(uint32 size)
    {
        cvars = new CVarStorage<T>[size]();
    }
    ~CVarArray()
    {
        delete cvars;
    }

    T* GetCurrentPtr(int32_t index)
	{
		return &cvars[index].current;
	};

    T GetCurrent(int32 index)
    {
        return cvars[index].current;
    };

    void SetCurrent(cont T& val, int32 index)
    {
        cvars[index].current = val;
    }

    int Add(const T& val, CVarParameter* param)
    {
        int index = lastCvar;
        cvars[index].current = val;
        cvars[index].initial = val;
        cvars[index].parameter = param;

        param->arrayIndex =index;
        last_cvar++;
        return index;
    }

        int Add(const T& init_val, const T& val, CVarParameter* param)
    {
        int index = lastCvar;
        cvars[index].current = val;
        cvars[index].initial = init_val;
        cvars[index].parameter = param;

        param->arrayIndex =index;
        last_cvar++;
        return index;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IMPLEMENTATION CLASS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class CVarSystemImpl : public CVarSystem
{
public:
// CVar Storage
    constexpr static int MAX_INT_CVARS = 1000;
    CVarArray<int32> int_cvars{MAX_INT_CVARS}; // Bracked Syntax will initalize in order they data fields... neat!

    constexpr static int MAX_FLOAT_CVARS;
    CVarArray<float> float_cvars{MAX_FLOAT_CVARS};

    constexpr static int MAX_STRING_CVARS = 200;
    CVarArray<std::string> string_cvars{MAX_STRING_CVARS};

    template<typename T>
    CVarArray<T>* GetCVarArray();

    template<>
    CVarArray<int32>* GetCVarArray()
    {
        return &int_cvars;
    }

    template<>
    CVarArray<float>* GetCVarArra()
    {
        return &float_cvars;
    }

    template<>
    CVarArray<std::string>* GetCVarArray()
    {
        return &string_cvars;
    }
// CVar Modification
    float* GetFloatCVar(StringUtils::StringHash hash) override final;
    int32* GetIntCVar(StringUtils::StringHash hash) override final;
    const char* GetStringCVar(StringUtils::StringHash hash) override final;

    float* GetFloatCVar(StringUtils::StringHash hash, float val) override final;
    int* GetIntCVar(StringUtils::StringHash hash, int32 val) override final;
    const char* GetStringCVar(StringUtils::StringHash hash, const char* val) override final;
    
    template<typename T>
    T* GetCVarCurrent(uint32 name_hash)
    {
        CVarParameter* param = GetCVar(name_hash);
        if(!cvar) return NULL;

        return GetCVarArray<T>()->GetCurrentPtr(param->arrayIndex);
    }

    template<typename T>
    T* GetCVarCurrent(uint32 name_hash, const T& val)
    {
        CVarParameter* cvar = GetCVar(name_hash);
        if(!cvar) GetCVarArray<T>()->SetCurrent(val, cvar->arrayIndex);        
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTUCTORS / DESTRUCTORS ///////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVarSystem* CVarSystem::Get()
{
    static CVarSystemImpl cvar_sys{};
    return &cvar_sys;
}

CVarParameter* CVarSystemImpl::InitCVar(const char* name, const char* description)
{
    if(GetCVar(name)) return NULL;

    uint32 name_hash = StringUtils::StringHash{name};
    saved_cvars[name_hash] = CVarParameter{};

    CVarParameter& new_param = saved_cvars[name_hash];
    new_param.name = name;
    new_param.description = description;

    return &new_param;
}

// GETTERS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVarParameter* CVarSystemImpl::GetCVar(StringUtils::StringHash hash)
{
    auto it = saved_cvars.find(hash);
    if(it == saved_cvars.end()) return NULL

    return &(*it).second;
}

float* CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<float>(hash);
}

int32* CVarSystemImpl::GetIntCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<int32>(hash);
}

const char* CVarSystemImpl::GetStringCVar(StringUtils::StringHash hash)
{
    return GetCVarCurrent<std::string>(hash);
}

// SETTERS /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash, float val)
{
    SetCVarCurrent<float>(hash, val);
}

void CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash, int32 val)
{
    SetCVarCurrent<int32>(hash, val);
}

void CVarSystemImpl::GetFloatCVar(StringUtils::StringHash hash, const char* val)
{
    SetCVarCurrent<std::string>(hash, val);
}

// CREATORS ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CVarParameter* CVarSystemImpl::CreateFloatCVar(cosnt char* name, const char* description, float def_value, float curr_value)
{
    CVarParameter* param = InitCVar(name, description);
    if(!param) return NULL;

    param->type = CVarType::FLOAT;
    GetCVarArray<float>()->Add(def_value, curr_value, param);
    
    return param;
}

CVarParameter* CVarSystemImpl::CreateIntCVar(cosnt char* name, const char* description, int32 def_value, int32 curr_value)
{
    CVarParameter* param = InitCVar(name, description);
    if(!param) return NULL;

    param->type = CVarType::INT;
    GetCVarArray<int32>()->Add(def_value, curr_value, param);
    
    return param;
}

CVarParameter* CVarSystemImpl::CreateStringCVar(cosnt char* name, const char* description, const char* def_value, const char* curr_value)
{
    CVarParameter* param = InitCVar(name, description);
    if(!param) return NULL;

    param->type = CVarType::FLOAT;
    GetCVarArray<std::string>()->Add(def_value, curr_value, param);
    
    return param;
}