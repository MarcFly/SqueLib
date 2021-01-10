#ifdef SHADERS_SOLO
#   include "fly_shaders.h"
#else
#   include "fly_lib.h"
#endif

#include <glad/glad.h>

#ifdef USE_OPENGLES
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADERS ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// THIS HAS TO BE REVISED
// Probably generating Macros depending on API WILL BE WAAAY Better but more cumbersome
// example, generate a fly_rendermacros.h and add there the macros separately, just include if necessary
// INSTEAD of using enums and switches...

FLY_Shader* FLYSHADER_Create(int32 type, uint16 strs, const char** file, bool raw_string)
{
    const char** data = nullptr;
    if (raw_string)
    {
        data = file;
        // Load File into data
    }
    else
    {
        strs = 1;
        // load the data
    }

    if (data == nullptr)
    {
        FLYLOG(LT_WARNING, "Shader could not be found or read...");
        return nullptr;
    }
    FLY_Shader* ret = new FLY_Shader();
    ret->type = type;
    ret->source = data;
    ret->lines = strs;
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
    ret->id = glCreateShader(ret->type);
    glShaderSource(ret->id, strs, ret->source, NULL);
#endif

    return ret;
}

void FLY_Shader::Compile()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glShaderSource(id, lines, source, NULL);
    glCompileShader(id);

    FLYSHADER_CheckCompileLog(this);
#endif
}

void FLYSHADER_CheckCompileLog(FLY_Shader* shader)
{
    if (shader == nullptr)
    {
        FLYLOG(LT_WARNING, "No shader sent to be checked...");
        return;
    }
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetShaderiv(shader->id, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(shader->id, 512, NULL, infoLog);

#endif
    if(!success) FLYPRINT(LT_WARNING, "Shader Compilation Info: %s", infoLog);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

FLY_Uniform::FLY_Uniform(const char* name_)
{
    name = name_;
}

FLY_Program* FLYSHADER_CreateProgram(uint16 layout_flags)
{
    FLY_Program* ret = new FLY_Program();
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    ret->id = glCreateProgram();
#endif
    return ret;
}

void FLY_Program::Init()
{
    if (id == INT32_MAX)
    {
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        id = glCreateProgram();
#endif
    }
}

void FLY_Program::AttachShader(FLY_Shader** shader)
{
    if (shader == nullptr || *shader == nullptr)
    {
        FLYLOG(LT_WARNING, "No shader/program sent to attach...");
        return;
    }
    if ((*shader)->type == FLY_VERTEX_SHADER)
        vertex_s = *shader;
    else if ((*shader)->type == FLY_FRAGMENT_SHADER)
        fragment_s = *shader;

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glAttachShader(id, (*shader)->id);
#endif

    *shader = nullptr;
}

void FLY_Program::Link()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glLinkProgram(id);
#endif

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glDeleteShader(vertex_s->id); // Still held in memory for FLY_Shader to be reused
    glDeleteShader(fragment_s->id);
#endif

    FLYRENDER_CheckProgramLog(this);
}

// Has Errors
void FLY_Program::GiveAttributesFromMesh(FLY_Mesh* mesh)
{
    uint16 size = mesh->attributes.size();
    Use();
    mesh->Bind();
    uint16 vert_size = mesh->GetVertSize();
    for (int i = 0; i < size; ++i)
    {
        FLY_Attribute* atr = mesh->attributes[i];
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        atr->SetId(glGetAttribLocation(id, atr->name));
#endif
        atr->SetAttribute(vert_size);
        GiveAttribute(&atr);
        bool test1 = mesh->attributes[i] == nullptr;
        bool test2 = false;
    }
}
void FLY_Program::GiveAttribute(FLY_Attribute** attr)
{
    if (!attr || !*attr)
    {
        FLYPRINT(LT_WARNING, "Non-existing attribute...");
        return;
    }
    attributes.push_back(*attr);

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    int test = glGetAttribLocation(id, (*attr)->name);
    (*attr)->SetId(glGetAttribLocation(id, (*attr)->name));
#endif
    *attr = nullptr;
}

//void FLY_Program::UpdateAttribute(const char* name)
//{

//}

uint16 FLY_Program::GetAttribByteSize() const
{
    uint16 ret = 0;
    uint16 size = attributes.size();
    for (int i = 0; i < size; ++i)
        ret += attributes[i]->GetSize();

    return ret;
}

void FLY_Program::EnableOwnAttributes()
{
    uint16 size = attributes.size();
    uint16 stride = GetAttribByteSize();
    for (int i = 0; i < size; ++i)
    {
        FLY_Attribute* atr = attributes[i];
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        glEnableVertexAttribArray(atr->id);
        glVertexAttribPointer(atr->id, atr->num_comp, atr->var_type, atr->normalize, stride, (void*)atr->offset);
#endif
    }
}

// Has Errors
void FLY_Program::DeclareUniform(const char* name)
{
    FLY_Uniform* uni = new FLY_Uniform(name);
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    uni->id = glGetUniformLocation(id, uni->name);
#endif
    uniforms.push_back(uni);
}

#include <cstring>
int32 FLY_Program::GetUniformLocation(const char* name) const
{
    int size = uniforms.size();
    for (int i = 0; i < size; ++i)
        if (std::strcmp(name, uniforms[i]->name) == 0) 
            return uniforms[i]->id;

    return UINT32_MAX;
}

void FLY_Program::Use()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glUseProgram(id);

#endif
}

void FLY_Program::DrawIndices(FLY_Mesh* mesh, int32 offset_bytes, int32 count)
{    
    count = (count == 0) ? mesh->num_index : count;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_id);
    glDrawElements(GL_TRIANGLES, count, mesh->index_var, (void*)(mesh->index_var_size * offset_bytes));
#else
#endif
}

void FLY_Program::DrawRawVertices(FLY_Mesh* mesh, int32 count)
{
    count = (count == 0) ? mesh->num_verts : count;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vert_id);
    glDrawArrays(GL_TRIANGLES, 0, count);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAM PASSING UNIFORMS //////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLY_Program::SetBool(const char* name, bool value) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(GetUniformLocation(name), value);
#endif
}

void FLY_Program::SetInt(const char* name, int value) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(GetUniformLocation(name), value);
#endif
}

void FLY_Program::SetFloat(const char* name, float value) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1f(GetUniformLocation(name), value);
#endif
}

void FLY_Program::SetFloat2(const char* name, float2 value) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2f(GetUniformLocation(name), value.x, value.y);
#endif
}

void FLY_Program::SetFloat3(const char* name, float3 value) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
#endif
}

void FLY_Program::SetFloat4(const char* name, float4 value) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
#endif
}

void FLY_Program::SetMatrix4(const char* name, const float* value, uint16 number, bool transpose) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniformMatrix4fv(GetUniformLocation(name), number, transpose, value);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAM CLEANUP ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
FLY_Program::~FLY_Program() { CleanUp(); }
void FLY_Program::CleanUp()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    if (vertex_s != NULL && id && vertex_s->id) { glDetachShader(id, vertex_s->id); }
    if (fragment_s != NULL && id && fragment_s->id) { glDetachShader(id, fragment_s->id);}
    if (vertex_s != NULL && vertex_s->id) { glDeleteShader(vertex_s->id); vertex_s->id = 0; }
    if (fragment_s != NULL &&  fragment_s->id) { glDeleteShader(fragment_s->id); fragment_s->id = 0; }
    if (id) { glDeleteProgram(id); id = 0; }
#endif

    if (vertex_s != NULL) { delete vertex_s; vertex_s = NULL; }
    if (fragment_s != NULL) { delete fragment_s; fragment_s = NULL; }

    int size = uniforms.size();
    for (int i = 0; i < size; ++i)
        delete uniforms[i];
    uniforms.clear();
}

void FLYRENDER_CheckProgramLog(FLY_Program* prog)
{
    if (prog == nullptr)
    {
        FLYLOG(LT_WARNING, "No program sent to be checked...");
        return;
    }
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetProgramiv(prog->id, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(prog->id, 512, NULL, infoLog);

#endif
    if(!success) FLYLOG(LT_WARNING, "Program Linkage Info: %s", infoLog);
}