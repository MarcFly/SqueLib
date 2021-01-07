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
#if defined(USE_OPENGL) || defined(USE_OPENGLES)

int ReturnNativeShaderType(int type)
{
    switch ((FLY_ShaderTypes)type)
    {
    case FLYSHADER_VERTEX:
        return GL_VERTEX_SHADER;
    case FLYSHADER_FRAGMENT:
        return GL_FRAGMENT_SHADER;
    }

    return -1;
}
#endif

FLY_Shader* FLYSHADER_Create(int type, const char* file, bool raw_string)
{
    const char* data = file;
    if (!raw_string)
    {
        // Load File into data
    }

    if (data == nullptr)
    {
        FLYLOG(LT_WARNING, "Shader could not be found or read...");
        return nullptr;
    }
    FLY_Shader* ret = new FLY_Shader();
    ret->type = type;
    ret->source = data;
#if defined (USE_OPENGL) || defined(USE_OPENGLES)
    ret->id = glCreateShader(ReturnNativeShaderType(ret->type));
    glShaderSource(ret->id, 1, &ret->source, NULL);
#endif

    return ret;
}

void FLY_Shader::Compile()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glShaderSource(id, 1, &source, NULL);
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
    FLYLOG(LT_WARNING, "Shader Compilation Info: %s", infoLog);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAMS //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

FLY_Program* FLYSHADER_CreateProgram(uint16 layout_flags)
{
    FLY_Program* ret = new FLY_Program();

    SET_FLAG(ret->layout, layout_flags);

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    ret->id = glCreateProgram();
#endif

    return ret;
}

void FLY_Program::AttachShader(FLY_Shader** shader)
{
    if (shader == nullptr || *shader == nullptr)
    {
        FLYLOG(LT_WARNING, "No shader/program sent to attach...");
        return;
    }
    switch ((*shader)->type)
    {
    case FLYSHADER_VERTEX:
        vertex_s = *shader;
        break;
    case FLYSHADER_FRAGMENT:
        fragment_s = *shader;
        break;
    }

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



void FLY_Program::EnableAttributes(FLY_Buffer* buf)
{
    const int vert_size = buf->GetVertSize();
    int offset = 0;
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    // Verte Position
    GLint attr = glGetAttribLocation(id, "v_pos");
    glVertexAttribPointer(attr, buf->pos_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset*sizeof(float)));
    glEnableVertexAttribArray(attr);
    offset += buf->pos_size;
    // Vertex Color
    attr = glGetAttribLocation(id, "v_col");
    glVertexAttribPointer(attr, buf->color_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset*sizeof(float)));
    glEnableVertexAttribArray(attr);
    offset += buf->color_size;
    // Vertex UV
    attr = glGetAttribLocation(id, "v_uv");
    glVertexAttribPointer(attr, buf->uv_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset * sizeof(float)));
    glEnableVertexAttribArray(attr);
    offset += buf->uv_size;
    // Vertex Normals
    attr = glGetAttribLocation(id, "v_normal");
    glVertexAttribPointer(attr, buf->normal_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset * sizeof(float)));
    glEnableVertexAttribArray(attr);
    offset += buf->normal_size;

#endif
}

void FLY_Program::SetupUniformLocations()
{
    int size = uniform.size();
    for (int i = 0; i < size; ++i)
    {
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
        uniform[i]->id = glGetUniformLocation(id, uniform[i]->name);
#endif
    }
}

#include <cstring>
uint32 FLY_Program::GetUniformLocation(const char* name) const
{
    int size = uniform.size();
    for (int i = 0; i < size; ++i)
        if (std::strcmp(name, uniform[i]->name) == 0) return uniform[i]->id;

    return UINT32_MAX;
}

void FLY_Program::Prepare()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glUseProgram(id);
#endif
}

void FLY_Program::Draw(FLY_Buffer* buf)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glUseProgram(id);
    glBindVertexArray(buf->attribute_object);
#endif
    EnableAttributes(buf);
    if(buf->num_index > 0)
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        glDrawElements(GL_TRIANGLES, buf->num_index, GL_UNSIGNED_INT, 0);
#else
    {}
#endif
    else
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        glDrawArrays(GL_TRIANGLES, 0, ((CHK_FLAG(layout, FLYSHADER_SIZE_POS_2)? 2 : 3)));
#else
    {}
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PROGRAM CLEANUP ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
FLY_Program::~FLY_Program() { CleanUp(); }
void FLY_Program::CleanUp()
{
    if(vertex_s != nullptr) delete vertex_s;
    if(fragment_s != nullptr) delete fragment_s;

    int size = uniform.size();
    for (int i = 0; i < size; ++i)
        delete uniform[i];

    uniform.clear();
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
    FLYLOG(LT_WARNING, "Program Linkage Info: %s", infoLog);
}