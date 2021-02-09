#ifdef SHADERS_SOLO
#   include "sque_shaders.h"
#else
#   include "squelib.h"
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

SQUE_Shader::SQUE_Shader() : id(0), type(NULL), lines(0), source(NULL)
{}

SQUE_Shader::SQUE_Shader(int32_t type_, uint16_t strs, const char** data) :
    type(type_), lines(strs), source(data)
{
    if (data == NULL | *data == NULL)
    {
        SQUE_LOG(LT_WARNING, "Shader could not be found or read...");
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
        SQUE_SHADER_CheckCompileLog(*this);
    }
}

SQUE_Shader::~SQUE_Shader()
{
    CleanUp();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_Shader::Compile()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glShaderSource(id, lines, source, NULL);
    glCompileShader(id);
#endif
    SQUE_SHADER_CheckCompileLog(*this);
}

void SQUE_SHADER_CheckCompileLog(const SQUE_Shader& shader)
{
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(shader.id, 512, NULL, infoLog);

#endif
    if(!success) SQUE_PRINT(LT_WARNING, "Shader Compilation Info: %s", infoLog);
}

void SQUE_Shader::CleanUp()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glDeleteShader(id);
#endif
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// UNIFORMS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
SQUE_Uniform::SQUE_Uniform() : id(-1), name("")
{}

SQUE_Uniform::SQUE_Uniform(const char* name_) : id(-1), name(name_)
{}

SQUE_Uniform::~SQUE_Uniform()
{}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetBool(const SQUE_Program& prog, const char* name, bool value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(prog.GetUniformLocation(name), value);
#endif
}

void SetInt(const SQUE_Program& prog, const char* name, int value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(prog.GetUniformLocation(name), value);
#endif
}

/*
void SetInt2(const SQUE_Program& prog, const char* name, int value[2])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2i(prog.GetUniformLocation(name), value.x, value[2]);
#endif
}

void SetInt3(const SQUE_Program& prog, const char* name, int3 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3i(prog.GetUniformLocation(name), value.x, value.y, value.z);
#endif
}

void SetInt4(const SQUE_Program& prog, const char* name, int4 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4i(prog.GetUniformLocation(name), value.x, value.y, value.z, value.w);
#endif
}
*/

void SetFloat(const SQUE_Program& prog, const char* name, float value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1f(prog.GetUniformLocation(name), value);
#endif
}

void SetFloat2(const SQUE_Program& prog, const char* name, glm::vec2 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2f(prog.GetUniformLocation(name), value.x, value.y);
#endif
}

void SetFloat3(const SQUE_Program& prog, const char* name, glm::vec3 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3f(prog.GetUniformLocation(name), value.x, value.y, value.z);
#endif
}

void SetFloat4(const SQUE_Program& prog, const char* name, glm::vec4 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4f(prog.GetUniformLocation(name), value.x, value.y, value.z, value.w);
#endif
}

void SetMatrix4(const SQUE_Program& prog, const char* name, const float* value, uint16_t number_of_matrices, bool transpose)
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
SQUE_Program::SQUE_Program() : vertex_s(NULL), fragment_s(NULL) // other shaders,...
{}

SQUE_Program::~SQUE_Program() { if(id > 0) CleanUp(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_Shader* SQUE_Program::AttachShader(SQUE_Shader* shader)
{
    if (shader == NULL)
    {
        SQUE_LOG(LT_WARNING, "No shader/program sent to attach...");
        return NULL;
    }
    SQUE_Shader* ret = NULL;
    if (shader->type == SQUE_VERTEX_SHADER)
    {
        ret = vertex_s;
        vertex_s = shader;
    }
    else if (shader->type == SQUE_FRAGMENT_SHADER)
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

void SQUE_Program::FreeShadersFromGPU()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glDeleteShader(vertex_s->id);
    glDeleteShader(fragment_s->id);
#endif
}

void SQUE_Program::DeclareUniform(const char* name)
{
    SQUE_Uniform* uni = new SQUE_Uniform(name);
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    uni->id = glGetUniformLocation(id, uni->name);
#endif
    uniforms.push_back(uni);
}

#include <cstring>
int32_t SQUE_Program::GetUniformLocation(const char* name) const
{
    int size = uniforms.size();
    for (int i = 0; i < size; ++i)
        if (std::strcmp(name, uniforms[i]->name) == 0)
            return uniforms[i]->id;

    return UINT32_MAX;
}

void SQUE_Program::CleanUp()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    if (vertex_s != NULL && id && vertex_s->id) { glDetachShader(id, vertex_s->id); vertex_s->id = 0; }
    if (fragment_s != NULL && id && fragment_s->id) { glDetachShader(id, fragment_s->id); fragment_s->id = 0; }
    if (id > 0) { glDeleteProgram(id); id = 0; }
#endif

    vertex_s = NULL;
    fragment_s = NULL;

    int size = uniforms.size();
    for (int i = 0; i < size; ++i)
        delete uniforms[i];
    uniforms.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAM CLEANUP ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void SQUE_RENDER_CheckProgramLog(const SQUE_Program& prog)
{
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetProgramiv(prog.id, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(prog.id, 512, NULL, infoLog);

#endif
    if(!success) SQUE_LOG(LT_WARNING, "Program Linkage Info: %s", infoLog);
}