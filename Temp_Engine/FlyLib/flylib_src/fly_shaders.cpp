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
    FLYPRINT(LT_WARNING, "Shader Compilation Info: %s", infoLog);
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

void FLY_Program::Init(uint16 layout_flags)
{
    SET_FLAG(layout, layout_flags);

    if (id == UINT32_MAX)
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
void FLY_Program::EnableAttributes(FLY_Buffer* buf)
{
    if (buf == nullptr)
    {
        FLYLOG(LT_WARNING, "Invalid buffer, no attributes enabled");
        return;
    }
    const int vert_size = buf->GetVertSize();
    int offset = 0;
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    // PreBind the buffers for modification
    glBindVertexArray(buf->attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, buf->vert_id);
    if (buf->num_index > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->index_id);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    // Vertex Position
    uint16 size = 0;
    GLint attr = -1;
    if ((attr = glGetAttribLocation(id, "v_pos")) != -1)
    {
        size = 0; buf->GetPosSize();
        glEnableVertexAttribArray(attr);
        glVertexAttribPointer(attr, buf->pos_comp, buf->pos_var, buf->pos_norm, vert_size, (void*)(offset));
        offset += size;
    }
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    // Vertex Color
    if (attr = glGetAttribLocation(id, "v_color"))
    {
        size = buf->GetColorSize();
        glEnableVertexAttribArray(attr);
        glVertexAttribPointer(attr, buf->color_comp, buf->color_var, buf->color_norm, vert_size, (void*)(offset));
        offset += size;
    }    
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    // Vertex UV
    if ((attr = glGetAttribLocation(id, "v_uv")) != -1)
    {
        size = buf->GetUVSize();
        glEnableVertexAttribArray(attr);
        glVertexAttribPointer(attr, buf->uv_comp, buf->uv_var, buf->uv_norm, vert_size, (void*)(offset));
        offset += size;
    }
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    // Vertex Normals
    if (attr = glGetAttribLocation(id, "v_normal") != -1)
    {
        size = buf->GetNormalSize();
        glEnableVertexAttribArray(attr);
        glVertexAttribPointer(attr, buf->normal_comp, buf->normal_var, buf->normal_norm, vert_size, (void*)(offset));
        offset += size;
    }
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());

#endif
}

// Has Errors
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

void FLY_Program::Draw(FLY_Buffer* buf, int offset_bytes, int count)
{
    
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glUseProgram(id);
    glBindVertexArray(buf->attribute_object);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
#endif
    EnableAttributes(buf);
    if (buf->num_index > 0)
    {
        count = (count == 0) ? buf->num_index : count;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        glDrawElements(GL_TRIANGLES, count, buf->index_var, (void*)offset_bytes);
#else
#endif
    }
    else
    {
        count = (count == 0) ? buf->num_verts : count;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        glDrawArrays(GL_TRIANGLES, 0, count);
#else
#endif
    }
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
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