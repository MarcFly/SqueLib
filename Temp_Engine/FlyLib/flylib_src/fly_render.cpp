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

void FLY_Buffer::SetSizes(uint16 pos, uint16 col, uint16 uv, uint16 normal)
{
    pos_size = pos;
    color_size = col;
    uv_size = uv;
    normal_size = normal;
    // tangent, bitangent,... whtever needed
}

uint16 FLY_Buffer::GetVertSize()
{
    return pos_size + color_size + uv_size + normal_size /* tangent, bitangent,... whatever needed*/;
}

void FLY_Buffer::SetAttributes()
{
    int attribs = 0;

    int vert_size = GetVertSize();
    int offset = 0;
    
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    // Vertex Position
    if (pos_size > 0)
    {
        glVertexAttribPointer(attribs, pos_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset*sizeof(float)));
        glEnableVertexAttribArray(attribs);
        offset += pos_size;
    }   attribs++;
    // Vertex Color
    if(color_size > 0)
    {
        glVertexAttribPointer(attribs, color_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset*sizeof(float)));
        glEnableVertexAttribArray(attribs);
        offset += color_size;
    }   attribs++;
    // Vertex UV
    if(uv_size > 0)
    {
        glVertexAttribPointer(attribs, uv_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset*sizeof(float)));
        glEnableVertexAttribArray(attribs);
        offset += uv_size;
    }   attribs++;
    // Vertex Normals
    if (normal_size > 0)
    {
        glVertexAttribPointer(attribs, normal_size, GL_FLOAT, GL_FALSE, vert_size * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(attribs);
        offset += uv_size;
    }   attribs++;
    // Other required data...
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLY_Mesh::PrepareBuffers(uint16 num_buffers)
{
    if(ids != nullptr) delete ids;
    ids = new uint32[num_buffers];
    num_ids = num_buffers;

#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGenBuffers(num_buffers, ids);
#endif

    if(buffers == nullptr)
    {
        buffers = new FLY_Buffer*[num_buffers];

        for(int i = 0; i < num_buffers; ++i)
        {
            buffers[i] = new FLY_Buffer();
            FLY_Buffer* buf = buffers[i];
            buf->vert_id = ids[i];
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
            glGenVertexArrays(1, &buf->attribute_object);
#endif
        }
    }

}

void FLY_Mesh::SendToGPU()
{
    for(int i = 0; i < num_ids; ++i)
    {
        FLY_Buffer* buf = buffers[i];
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
        glBindVertexArray(buf->attribute_object);

        glBindBuffer(GL_ARRAY_BUFFER, ids[i]);
        int buffer_size = buf->GetVertSize() * buf->num_verts * sizeof(float);
        glBufferData(GL_ARRAY_BUFFER, buffer_size, buf->verts, GL_STATIC_DRAW);

        if (CHK_FLAG(buf->layout, FLYSHADER_LAYOUT_HAS_INDICES) || buf->num_index > 0)
        {
            glGenBuffers(1, &buf->index_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->index_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32)*buf->num_index, buf->indices, GL_STATIC_DRAW);
        }

#endif
        buffers[i]->SetAttributes();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG... //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLYRENDER_TestRender(FLY_Program& prog, FLY_Mesh& mesh)
{

    prog.Draw(mesh.buffers[0]);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
}

void GetGLError()
{
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
}