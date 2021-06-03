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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_SHADERS_GenerateID(SQUE_Shader* shader, const int32_t shader_type)
{
    shader->type = shader_type;
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
    shader->id = glCreateShader(shader_type);
#endif
}

void SQUE_SHADERS_SetSource(const int32_t shader_id, const char* source_)
{
    const char* strs[2] = { SQUE_RENDER_GetShaderHeader(), source_ };
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
    glShaderSource(shader_id, 2, strs, NULL);
#endif
}

void SQUE_SHADERS_Compile(int32_t shader_id)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glCompileShader(shader_id);
#endif
    SQUE_SHADERS_CheckCompileLog(shader_id);
}

void SQUE_SHADERS_FreeFromGPU(int32_t shader_id)
{
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
    glDeleteShader(shader_id);
#endif
}

void SQUE_SHADERS_CheckCompileLog(const int32_t shader_id)
{
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(shader_id, 512, NULL, infoLog);

#endif
    if(!success) SQUE_PRINT(LT_WARNING, "Shader Compilation Info: %s", infoLog);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_PROGRAM_GenerateID(uint32_t* program_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    *program_id = glCreateProgram();
#endif
}

void SQUE_PROGRAM_Link(const uint32_t program_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glLinkProgram(program_id);
#endif
}

void SQUE_PROGRAM_Use(const uint32_t program_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUseProgram(program_id);
#endif
}

void SQUE_PROGRAM_CacheUniforms(SQUE_Program* program)
{
    int32_t count;
    
    glGetProgramiv(program->id, GL_ACTIVE_UNIFORMS, &count);
    program->uniforms.clear();
    for (uint16_t i = 0; i < count; ++i)
    {
        program->uniforms.push_back(SQUE_Uniform());
        int32_t l;
        SQUE_Uniform* u = program->uniforms.last();
        u->id = i;
        glGetActiveUniform(program->id, i, sizeof(u->name), &l, &u->var_size, &u->type, u->name);
    }
}


int32_t SQUE_PROGRAM_GetUniformID(const SQUE_Program& program, const char* name)
{
    const sque_vec<SQUE_Uniform>& uniforms = program.uniforms;
    for (uint16_t i = 0; i < uniforms.size(); ++i)
        if (strcmp(uniforms[i].name, name) == 0) return uniforms[i].id;
    return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SetBool(const int32_t uniform_id, bool value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(uniform_id, value);
#endif
}

void SetInt(const int32_t uniform_id, int32_t value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1i(uniform_id, value);
#endif
}

/*
void SetInt2(const int32_t uniform_id, int value[2])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2i(uniform_id, value.x, value[2]);
#endif
}

void SetInt3(const int32_t uniform_id, int3 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3i(uniform_id, value.x, value.y, value.z);
#endif
}

void SetInt4(const int32_t uniform_id, int4 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4i(uniform_id, value.x, value.y, value.z, value.w);
#endif
}
*/

void SetFloat(const int32_t uniform_id, const float value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1f(uniform_id, value);
#endif
}

void SetFloat2(const int32_t uniform_id, const float value[2])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2f(uniform_id, value[0], value[1]);
#endif
}

void SetFloat3(const int32_t uniform_id, const float value[3])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3f(uniform_id, value[0], value[1], value[2]);
#endif
}

void SetFloat4(const int32_t uniform_id, const float value[4])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4f(uniform_id, value[0], value[1], value[2], value[3]);
#endif
}

void SetMatrix4(const int32_t uniform_id, const float* value, uint16_t number_of_matrices, bool transpose)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniformMatrix4fv(uniform_id, number_of_matrices, transpose, value);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADER PROGRAM ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_PROGRAM_AttachShader(SQUE_Program* program, const SQUE_Shader& shader)
{
    int32_t pos = -1 
        + (shader.type == SQUE_VERTEX_SHADER) * 1
        + (shader.type == SQUE_FRAGMENT_SHADER) * 2;

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glAttachShader(program->id, shader.id);
#endif

    program->shaders[pos] = shader.id;
}

void SQUE_PROGRAM_FreeShadersFromGPU(int32_t shaders[])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    for(uint16_t i = 0; i < sizeof(shaders)/sizeof(shaders[0]); ++i)
        glDeleteShader(shaders[i]);
#endif
}

int32_t SQUE_PROGRAM_GetUniformLocation(const uint32_t id, const char* name)
{
    int32_t ret = -1;
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    ret = glGetUniformLocation(id, name);
#endif
    return ret;
}

void SQUE_PROGRAM_FreeFromGPU(int32_t program_id)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glDeleteProgram(program_id);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAM CLEANUP ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////


void SQUE_PROGRAM_CheckLinkLog(const uint32_t program_id)
{
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(program_id, 512, NULL, infoLog);

#endif
    if(!success) SQUE_LOG(LT_WARNING, "Program Linkage Info: %s", infoLog);
}