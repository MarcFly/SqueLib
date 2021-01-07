#ifdef RENDER_SOLO
#   include "fly_render.h"
#else
#   include "fly_lib.h"
#endif

#include <glad/glad.h>

#ifdef USE_OPENGLES
#include <GLES3/gl32.h>
#include <EGL/egl.h>
#endif
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION AND STATE CONTROL //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Returns the size of the main viewport/context
void FLYRENDER_ViewportSizeCallback(int width, int height)
{
    glViewport(0,0, width, height);
}

#ifdef USE_GLFW
#   include <GLFW/glfw3.h>
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    FLYRENDER_ViewportSizeCallback(width, height);
}

#endif

bool FLYRENDER_Init()
{
    bool ret = true;

#ifdef USE_OPENGL
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        FLYLOG(FLY_LogType::LT_CRITICAL, "Couldn't Initialize GLAD...");
        return false;
    }
#elif defined USE_OPENGLES
    if(!gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress))
    {
        FLYLOG(FLY_LogType::LT_CRITICAL, "Couldn't Initialize GLAD...");
        return false;
        //gladLoadGL();   
    }
#endif
    

    FLYLOG(FLY_LogType::LT_INFO, "GLAD Initialized!");
#ifdef USE_OPENGL
    FLYLOG(FLY_LogType::LT_INFO, "OpenGL/ES Version: %d.%d", GLVersion.major, GLVersion.minor);
#elif defined USE_OPENGLES
    FLYLOG(FLY_LogType::LT_INFO, "OpenGLES Version: %d.%d", GLVersion.major, GLVersion.minor);
#endif
    // Generate Viewport with window size
    uint16 w, h;
    FLYDISPLAY_GetSize(0, &w, &h);
    glViewport(0, 0, w, h);
    FLYLOG(FLY_LogType::LT_INFO, "Main Viewport init...");
    return ret;
}

void FLYRENDER_Close()
{

}

void FLYRENDER_Clear(int clear_flags, ColorRGBA* color)
{
    if(color != NULL) glClearColor(color->r, color->g, color->b, color->a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

const char* FLYRENDER_GetGLSLVer()
{
    return "#version 330";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// BUFFER MANAGEMENT /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLYRENDER_GenBuffer(FLY_Mesh* mesh, uint16 num_buffers)
{
    if(mesh == nullptr || num_buffers == 0)
    {
        FLYLOG(LT_WARNING, "No mesh sent to generate buffers for...");
        return;
    }

    if(mesh->ids != nullptr) delete mesh->ids;
    mesh->ids = new uint32[num_buffers];
    mesh->num_ids = num_buffers;

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGenBuffers(num_buffers, mesh->ids);
#endif

    if(mesh->buffers == nullptr)
    {
        mesh->buffers = new FLY_Buffer*[num_buffers];

        for(int i = 0; i < num_buffers; ++i)
        {
            mesh->buffers[i] = new FLY_Buffer();
            FLY_Buffer* buf = mesh->buffers[i];
            buf->vert_id = mesh->ids[i];
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
            glGenVertexArrays(1, &buf->attribute_object);
#endif
        }
    }

}

void FLYRENDER_BufferArray(FLY_Mesh* mesh)
{
    if(mesh == nullptr)
    {
        FLYLOG(LT_WARNING, "No mesh sent to buffer data...");
        return;
    }
    else if(mesh->buffers == nullptr || mesh->buffers[0]->vert_id == UINT32_MAX || mesh->buffers[0]->attribute_object == UINT32_MAX)
    {
        FLYLOG(LT_WARNING, "Buffers have not been generated...");
        return;
    }
    for(int i = 0; i < mesh->num_ids; ++i)
    {
        FLY_Buffer* buf = mesh->buffers[i];
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
        glBindVertexArray(buf->attribute_object);

        glBindBuffer(GL_ARRAY_BUFFER, mesh->ids[i]);
        int buffer_size = buf->vert_size * buf->num_verts * sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, buffer_size, buf->verts, GL_STATIC_DRAW);

        if (CHK_FLAG(buf->buffer_structure, FLYBUFFER_HAS_INDICES) || buf->num_index == 0)
        {
            glGenBuffers(1, &buf->index_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->index_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32)*buf->num_index, buf->indices, GL_STATIC_DRAW);
        }

#endif

        FLYRENDER_SetAttributesForBuffer(mesh->buffers[i]);
    }
}

void FLYRENDER_SetAttributesForBuffer(FLY_Buffer* buf)
{
    int attribs = 0;
    
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glVertexAttribPointer(attribs, buf->vert_size, GL_FLOAT, GL_FALSE, buf->vert_size*sizeof(float), (void*)0);
    // Remember to check back on this function to accept other attributes, how to setup the stride, position,...
    // Because the buffer right now just packs all vertex sent and that is that, no need to care but will be required.
    glEnableVertexAttribArray(attribs);
    attribs++;
#endif
    
    if (CHK_FLAG(buf->buffer_structure, FLYBUFFER_HAS_INDICES))
    {
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADERS ///////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(USE_OPENGL) || defined(USE_OPENGLES)

int ReturnNativeShaderType(int type)
{
    switch((FLY_ShaderTypes)type)
    {
        case FLYSHADER_VERTEX:
            return GL_VERTEX_SHADER;
        case FLYSHADER_FRAGMENT:
            return GL_FRAGMENT_SHADER;
    }

    return -1;
}
#endif
void FLYRENDER_CreateShader(FLY_Shader* shader)
{
    if(shader == nullptr)
    {
        FLYLOG(LT_WARNING, "No shader sent to be created...");
        return;
    }
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    shader->id = glCreateShader(ReturnNativeShaderType(shader->type));
    glShaderSource(shader->id, 1, &shader->source, NULL);
#endif
}

void FLYRENDER_CompileShader(FLY_Shader* shader)
{
    if(shader == nullptr)
    {
        FLYLOG(LT_WARNING, "No shader sent to be compiled...");
        return;
    }
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glShaderSource(shader->id, 1, &shader->source, NULL);
    glCompileShader(shader->id);

    FLYRENDER_CheckShaderLog(shader);
#endif
}

void FLYRENDER_CheckShaderLog(FLY_Shader* shader)
{
    if(shader == nullptr)
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

void FLYRENDER_CreateShaderProgram(FLY_Program* prog)
{
    if(prog == nullptr)
    {
        FLYLOG(LT_WARNING, "No program sent to be created...");
        return;
    }
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    prog->id = glCreateProgram();
#endif
}

void FLYRENDER_AttachShaderToProgram(FLY_Shader* shader, FLY_Program* prog)
{
    if(shader == nullptr || prog == nullptr)
    {
        FLYLOG(LT_WARNING, "No shader/program sent to attach...");
        return;
    }
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glAttachShader(prog->id, shader->id);
#endif
}

void FLYRENDER_AttachMultipleShadersToProgram(uint16 num_shaders, FLY_Shader** shaders, FLY_Program* prog)
{
    if(shaders == nullptr || num_shaders == 0 || prog == nullptr)
    {
        FLYLOG(LT_WARNING, "No shaders/program sent to attach...");
        return;
    }
    if (prog->shaders != nullptr) delete prog->shaders;
    prog->shaders = new FLY_Shader*[num_shaders];
    prog->num_shaders = num_shaders;
    for(int i = 0; i < num_shaders; ++i)
    {
        prog->shaders[i] = shaders[i];
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
        glAttachShader(prog->id, shaders[i]->id);
#endif
    }
}

void FLYRENDER_LinkShaderProgram(FLY_Program* prog)
{
    if(prog == nullptr)
    {
        FLYLOG(LT_WARNING, "No program sent to be linked...");
        return;
    }
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glLinkProgram(prog->id);
#endif

    for (int i = 0; i < prog->num_shaders; ++i)
    {
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        glDeleteShader(prog->shaders[i]->id); // Still held in memory for FLY_Shader to be reused
#endif
    }
    FLYRENDER_CheckProgramLog(prog);
}

void FLYRENDER_CheckProgramLog(FLY_Program* prog)
{
    if(prog == nullptr)
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

void FLYRENDER_ProgramEnableAttributes(FLY_Program* prog)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    GLint attr = glGetAttribLocation(prog->id, "aPos");
    glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // Remember to check back on this function to accept other attributes, how to setup the stride, position,...
    // Because the buffer right now just packs all vertex sent and that is that, no need to care but will be required.
    glEnableVertexAttribArray(attr);
#endif
    if(CHK_FLAG(prog->program_structure, FLYBUFFER_HAS_INDICES))
    {   
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
        /*attr = glGetAttribLocation(prog->id, "aPos");
        glVertexAttribPointer(attr, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        // Remember to check back on this function to accept other attributes, how to setup the stride, position,...
        // Because the buffer right now just packs all vertex sent and that is that, no need to care but will be required.
        glEnableVertexAttribArray(attr);*/
#endif
    }
}




void FLYRENDER_TestRender(FLY_Program& prog, FLY_Mesh& mesh)
{
    glUseProgram(prog.id);
    glBindVertexArray(mesh.buffers[0]->attribute_object);
    FLYRENDER_ProgramEnableAttributes(&prog);
    if (CHK_FLAG(mesh.buffers[0]->buffer_structure, FLYBUFFER_HAS_INDICES)) 
        glDrawElements(GL_TRIANGLES, mesh.buffers[0]->num_index, GL_UNSIGNED_INT, 0);
    else 
        glDrawArrays(GL_TRIANGLES, 0, 3);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
}

void GetGLError()
{
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
}