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

        if (CHK_FLAG(buf->layout, FLYSHADER_LAYOUT_HAS_INDICES) || buf->num_index == 0)
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
    
    if (CHK_FLAG(buf->layout, FLYSHADER_LAYOUT_HAS_INDICES))
    {
    }
}




void FLYRENDER_TestRender(FLY_Program& prog, FLY_Mesh& mesh)
{

    prog.Draw(mesh.buffers[0]->attribute_object, mesh.buffers[0]->num_index);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
}

void GetGLError()
{
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
}