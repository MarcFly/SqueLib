#ifdef SHADERS_SOLO
#   include "fly_shaders.h"
#else
#   include "fly_lib.h"
#endif

#include <glad/glad.h>

#ifdef USE_OPENGLES
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADERS ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

FLY_Shader::FLY_Shader() : id(0), type(NULL), lines(0), source(NULL)
{}

FLY_Shader::FLY_Shader(int32_t type_, uint16_t strs, const char** data) :
    type(type_), lines(strs), source(data)
{
    if (data == NULL | *data == NULL)
    {
        FLYLOG(LT_WARNING, "Shader could not be found or read...");
    }
    else
    {
        type = type;
        source = data;
        lines = strs;
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
        id = glCreateShader(type);
        glShaderSource(id, strs, source, NULL);
        glCompileShader(id);
#endif
        FLYSHADER_CheckCompileLog(*this);
    }
}

FLY_Shader::~FLY_Shader()
{}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLY_Shader::Compile()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glShaderSource(id, lines, source, NULL);
    glCompileShader(id);
#endif
    FLYSHADER_CheckCompileLog(*this);
}

void FLYSHADER_CheckCompileLog(const FLY_Shader& shader)
{
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(shader.id, 512, NULL, infoLog);

#endif
    if(!success) FLYPRINT(LT_WARNING, "Shader Compilation Info: %s", infoLog);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// UNIFORMS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
FLY_Uniform::FLY_Uniform() : id(-1), name("")
{}

FLY_Uniform::FLY_Uniform(const char* name_) : id(-1), name(name_)
{}

FLY_Uniform::~FLY_Uniform()
{}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetBool(const FLY_Program& prog, const char* name, bool value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(prog.GetUniformLocation(name), value);
#endif
}

void SetInt(const FLY_Program& prog, const char* name, int value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(prog.GetUniformLocation(name), value);
#endif
}

/*
void SetInt2(const FLY_Program& prog, const char* name, int value[2])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2i(prog.GetUniformLocation(name), value.x, value[2]);
#endif
}

void SetInt3(const FLY_Program& prog, const char* name, int3 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3i(prog.GetUniformLocation(name), value.x, value.y, value.z);
#endif
}

void SetInt4(const FLY_Program& prog, const char* name, int4 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4i(prog.GetUniformLocation(name), value.x, value.y, value.z, value.w);
#endif
}
*/

void SetFloat(const FLY_Program& prog, const char* name, float value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1f(prog.GetUniformLocation(name), value);
#endif
}

void SetFloat2(const FLY_Program& prog, const char* name, glm::vec2 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2f(prog.GetUniformLocation(name), value.x, value.y);
#endif
}

void SetFloat3(const FLY_Program& prog, const char* name, glm::vec3 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3f(prog.GetUniformLocation(name), value.x, value.y, value.z);
#endif
}

void SetFloat4(const FLY_Program& prog, const char* name, glm::vec4 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4f(prog.GetUniformLocation(name), value.x, value.y, value.z, value.w);
#endif
}

void SetMatrix4(const FLY_Program& prog, const char* name, const float* value, uint16_t number_of_matrices, bool transpose)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniformMatrix4fv(prog.GetUniformLocation(name), number_of_matrices, transpose, value);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADER PROGRAM ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
FLY_Program::FLY_Program() : vertex_s(NULL), fragment_s(NULL) // other shaders,...
{}

FLY_Program::~FLY_Program() { if(id > 0) CleanUp(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

FLY_Shader* FLY_Program::AttachShader(FLY_Shader* shader)
{
    if (shader == NULL)
    {
        FLYLOG(LT_WARNING, "No shader/program sent to attach...");
        return NULL;
    }
    FLY_Shader* ret = NULL;
    if (shader->type == FLY_VERTEX_SHADER)
    {
        ret = vertex_s;
        vertex_s = shader;
    }
    else if (shader->type == FLY_FRAGMENT_SHADER)
    {
        ret = fragment_s;
        fragment_s = shader;
    }

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    if (ret != NULL) glDetachShader(id, ret->id);
    glAttachShader(id, shader->id);
#endif

    return ret;
}

void FLY_Program::FreeShadersFromGPU()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glDeleteShader(vertex_s->id);
    glDeleteShader(fragment_s->id);
#endif
}

void FLY_Program::DeclareUniform(const char* name)
{
    FLY_Uniform* uni = new FLY_Uniform(name);
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    uni->id = glGetUniformLocation(id, uni->name);
#endif
    uniforms.push_back(uni);
}

#include <cstring>
int32_t FLY_Program::GetUniformLocation(const char* name) const
{
    int size = uniforms.size();
    for (int i = 0; i < size; ++i)
        if (std::strcmp(name, uniforms[i]->name) == 0)
            return uniforms[i]->id;

    return UINT32_MAX;
}

void FLY_Program::CleanUp()
{
    FLY_CHECK_RENDER_ERRORS();
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    if (vertex_s != NULL && vertex_s->id) { glDeleteShader(vertex_s->id); }
    if (fragment_s != NULL && fragment_s->id) { glDeleteShader(fragment_s->id); }
    if (vertex_s != NULL && id && vertex_s->id) { glDetachShader(id, vertex_s->id); vertex_s->id = 0; }
    if (fragment_s != NULL && id && fragment_s->id) { glDetachShader(id, fragment_s->id); fragment_s->id = 0; }
    if (id > 0) { glDeleteProgram(id); id = 0; }
#endif

    if (vertex_s != NULL) { delete vertex_s; vertex_s = NULL; }
    if (fragment_s != NULL) { delete fragment_s; fragment_s = NULL; }

    int size = uniforms.size();
    for (int i = 0; i < size; ++i)
        delete uniforms[i];
    uniforms.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAM CLEANUP ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void FLYRENDER_CheckProgramLog(const FLY_Program& prog)
{
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetProgramiv(prog.id, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(prog.id, 512, NULL, infoLog);

#endif
    if(!success) FLYLOG(LT_WARNING, "Program Linkage Info: %s", infoLog);
}