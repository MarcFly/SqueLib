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
SQUE_Shader::SQUE_Shader(int32_t type_, const char* source_) :
    type(type_)
{    
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
    id = glCreateShader(type);
    glShaderSource(id, 1, &source_, NULL);
#endif
}

SQUE_Shader::~SQUE_Shader()
{
    SQUE_SHADERS_FreeFromGPU(id);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_SHADERS_ChangeSource(int32_t shader_id, const char* source_)
{
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
    glShaderSource(shader_id, 1, &source_, NULL);
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
#include <unordered_set>
std::vector<SQUE_Uniform> uniforms;
std::vector<SQUE_ProgramUniforms> programs;

void SQUE_SHADERS_DeclareProgram(int32_t program_id, uint32_t num_uniforms)
{
    // TEST push/resize branching/if: 1 million ifs to check need for reserve and 1 million reserve with same capacity
    // If need reserve, reserve like 10-50 space
    int cap = programs.capacity() - 1; 
    programs.resize((cap < program_id) ? program_id : cap+1);
    SQUE_ProgramUniforms p;
    p.id = program_id;
    
    p.start_uniform = uniforms.capacity();
    p.end_uniform = p.start_uniform + num_uniforms - 1;
    uniforms.resize(p.start_uniform + num_uniforms);

    programs[program_id-1] = p;
}

int32_t SQUE_SHADERS_DeclareUniform(int32_t program_id, const char* name)
{
    SQUE_ProgramUniforms& p = programs[program_id-1];

    SQUE_Uniform uni;
    memcpy(uni.name, name, strlen(name));
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    uni.id = glGetUniformLocation(program_id, uni.name);
#endif
    uniforms[p.start_uniform + p.last] = uni;
    ++p.last;

    return uni.id;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// UNIFORMS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

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

void SetFloat(const int32_t uniform_id, float value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform1f(uniform_id, value);
#endif
}

void SetFloat2(const int32_t uniform_id, glm::vec2 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform2f(uniform_id, value.x, value.y);
#endif
}

void SetFloat3(const int32_t uniform_id, glm::vec3 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform3f(uniform_id, value.x, value.y, value.z);
#endif
}

void SetFloat4(const int32_t uniform_id, glm::vec4 value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUniform4f(uniform_id, value.x, value.y, value.z, value.w);
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

void SQUE_PROGRAM_AttachShader(SQUE_Program& program, int32_t shader_id, uint32_t type)
{
    int32_t pos = -1 + (type == SQUE_VERTEX_SHADER) * 1 + (type == SQUE_GEOMETRY_SHADER) * 2 + (type == SQUE_FRAGMENT_SHADER) * 3;

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    //glDetachShader(program.id, program.shaders[pos]); // No issues detaching non existing shader, just make sure
    glAttachShader(program.id, shader_id);
#endif

    program.shaders[pos] = shader_id;
}

void SQUE_PROGRAM_FreeShadersFromGPU(int32_t shaders[])
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    for(uint16_t i = 0; i < 3; ++i) // HARDOCODED: number of supported shaders per programs
        glDeleteShader(shaders[i]);
#endif
}

#include <cstring>
int32_t SQUE_PROGRAM_GetUniformLocation(const uint32_t program_id, const char* name)
{
    SQUE_ProgramUniforms& p = programs[program_id-1];

    uint32_t last = p.start_uniform + p.last;
    for (int i = p.start_uniform; i < last; ++i)
        if (std::strcmp(name, uniforms[i].name) == 0)
            return uniforms[i].id;

    return UINT32_MAX;
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


void SQUE_RENDER_CheckProgramLog(const uint32_t program_id)
{
    int success;
    char infoLog[512];

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(program_id, 512, NULL, infoLog);

#endif
    if(!success) SQUE_LOG(LT_WARNING, "Program Linkage Info: %s", infoLog);
}