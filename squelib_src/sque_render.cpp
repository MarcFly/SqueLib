#ifdef RENDER_SOLO
#   include "sque_render.h"
#else
#   include "squelib.h"
#endif

#if defined(USE_OPENGL)
#   include <glad/glad.h>
#elif defined(USE_OPENGLES)
#   include <GLES3/gl32.h>
#   include <GLES3/gl3ext.h>
#   include <EGL/egl.h>
#endif

static char glsl_ver[64] = {'\0'};
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION AND STATE CONTROL //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

ColorRGBA::ColorRGBA() {}
ColorRGBA::ColorRGBA(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
ColorRGBA::~ColorRGBA() {}

SQUE_RenderState::SQUE_RenderState() {}
SQUE_RenderState::~SQUE_RenderState() {}

void SQUE_RenderState::SetUp()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBlendEquationSeparate(blend_equation_rgb, blend_equation_alpha);
    if (blend_func_separate)
        glBlendFuncSeparate(blend_func_src_rgb, blend_func_dst_rgb, blend_func_src_alpha, blend_func_dst_alpha);
    else
        glBlendFunc(blend_func_src_alpha, blend_func_dst_alpha);

    if (blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    if (cull_faces) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    if (scissor_test) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);

    // OpenGL Version Specific Code
#   ifdef USE_OPENGL
    glPolygonMode(SQUE_FRONT_AND_BACK, (GLenum)polygon_mode[0]);
#   endif

    glBindFramebuffer(SQUE_DRAW_FBO, draw_framebuffer);
    glBindFramebuffer(SQUE_READ_FBO, read_framebuffer);
#endif

    //SQUE_CHECK_RENDER_ERRORS();
}

void SQUE_RenderState::BackUp()
{
    backed_up = true;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blend_equation_alpha);

    glGetIntegerv(GL_BLEND_SRC_RGB, &blend_func_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &blend_func_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_func_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_func_dst_alpha);

    blend = glIsEnabled(GL_BLEND);
    cull_faces = glIsEnabled(GL_CULL_FACE);
    depth_test = glIsEnabled(GL_DEPTH_TEST);
    scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // OpenGL Version Specific Code
#   ifdef USE_OPENGL
    glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
#   endif

    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &draw_framebuffer);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &read_framebuffer);
#endif

    //SQUE_CHECK_RENDER_ERRORS();
}

void SQUE_RENDER_ChangeFramebufferSize(int32_t width, int32_t height)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glViewport(0, 0, width, height);
#endif
}

void SQUE_RENDER_GetFramebufferSize(int32_t* width, int32_t* height)
{
    int32_t size[4];
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    
    glGetIntegerv(GL_VIEWPORT, size);
#endif

    *width = size[2];
    *height = size[3];
}

bool SQUE_RENDER_Init()
{
    bool ret = true;

#if defined(USE_OPENGL)
    if(!gladLoadGL())
    {
        SQUE_PRINT(SQUE_LogType::LT_CRITICAL, "Couldn't Initialize GLAD...");
        return false;
    }
#elif defined(USE_OPENGLES)
    if(!gladLoadGLES2Loader((GLADloadproc)eglGetProcAddress))
    {
        SQUE_PRINT(SQUE_LogType::LT_CRITICAL, "Couldn't Initialize GLAD...");
        //return false;
        gladLoadGL();   
    }
#endif
    

    SQUE_PRINT(SQUE_LogType::LT_INFO, "GLAD Initialized!");
#ifdef USE_OPENGL
    SQUE_PRINT(SQUE_LogType::LT_INFO, "OpenGL Core Version: %d.%d", GLVersion.major, GLVersion.minor);
#elif defined USE_OPENGLES
    SQUE_PRINT(SQUE_LogType::LT_INFO, "OpenGLES Version: %d.%d", GLVersion.major, GLVersion.minor);
#endif

#if defined(USE_OPENGL) ||defined(USE_OPENGLES)
    int ver = GLVersion.major * 100 + GLVersion.minor * 10;
    SQ_ASSERT(ver > 320 && "Minimum version is 320 es");
    const char* term = "\n";
#if defined(USE_OPENGLES)
    term = " es\nprecision mediump float;\n";
#else 
    term = " core\n";
#endif
    sprintf(glsl_ver, "#version %d %s", ver, term);  
#endif
    SQUE_PRINT(LT_INFO, "%s", glsl_ver);
    SQUE_PRINT(SQUE_LogType::LT_INFO, "Main Viewport init...");
    return ret;
}

void SQUE_RENDER_Close()
{

}

void SQUE_RENDER_Clear(const ColorRGBA& color, int clear_flags)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(clear_flags);
#endif
}

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
int32_t GetGLVer()
{
    return GLVersion.major * 100 + GLVersion.minor * 10;
}
#endif

// should it?
#include <stdio.h>
// Generar una global la string generada por esta funcion
// En un array de tama�o fijo y lo rellenas cn un memcpy y ale
// GetGLSLVer que devuelva la string.
const char* SQUE_RENDER_GetShaderHeader()
{
    return glsl_ver;
}

void SQUE_RENDER_Scissor(int x, int y, int w, int h)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glScissor(x, y, w, h);
#endif
}

void SQUE_RENDER_GetViewport(int32_t* x, int32_t* y, int32_t* w, int32_t* h)
{
    int vint[4];
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGetIntegerv(GL_VIEWPORT, vint);
#endif
    *x = vint[0];
    *y = vint[1];
    *w = vint[2];
    *h = vint[3];
}
void SQUE_RENDER_GetIntV(int32_t value_id, int32_t* value)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES) 
    glGetIntegerv(value_id, value);
#endif
}

void SQUE_RENDER_SetPolyMode(int32_t faces, int32_t mode)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES) 
    glPolygonMode(faces, mode);
#endif
}

void SQUE_RENDER_SetViewport(int x, int y, int w, int h)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glViewport(x,y,w,h);
#endif
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_MESH_GenBufferIDs(const uint32_t num, uint32_t* ids)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenBuffers(1, ids);
#else
#endif
}

void SQUE_MESH_GenAttributeObjects(const uint32_t num, uint32_t* attribute_objects)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenVertexArrays(1, attribute_objects);
#else
#endif
}


void SQUE_MESH_GenBuffer(SQUE_Mesh* mesh)
{
    // In debug, checking for invalid memory
    SQ_ASSERT(mesh != nullptr || mesh->vert_id || mesh->index_id || mesh->attribute_object); 
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenBuffers(1, &mesh->vert_id);
    glGenBuffers(1, &mesh->index_id);
    glGenVertexArrays(1, &mesh->attribute_object);
#endif
}

void SQUE_MESH_BindBuffer(const SQUE_Mesh& mesh)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(mesh.attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_id);
#endif
}

void SQUE_MESH_BindVertices(const uint32_t vert_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindBuffer(GL_ARRAY_BUFFER, vert_id);
#endif
}

void SQUE_MESH_BindIndices(const uint32_t index_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
#endif
}

void SQUE_MESH_BindAttributeObject(const uint32_t attribute_object)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
#endif
}

void SQUE_MESH_SendVertsToGPU(const SQUE_Mesh& mesh, void* vertices)
{
    int buffer_size = mesh.vertex_size * mesh.num_verts;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBufferData(GL_ARRAY_BUFFER, buffer_size, vertices, mesh.draw_mode);
#endif
}
void SQUE_MESH_SendIndicesToGPU(const SQUE_Mesh& mesh, void* indices)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.num_index * mesh.index_var_size, indices, mesh.draw_mode);
#endif
}

void SQUE_MESH_SendToGPU(const SQUE_Mesh& mesh, void* vertices, void* indices)
{
    SQUE_MESH_SendVertsToGPU(mesh, vertices);
    if (indices != NULL) SQUE_MESH_SendIndicesToGPU(mesh, indices);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VERTEX ATTRIBUTE MANAGEMENT ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_PROGRAM_EnableAttribute(const SQUE_Program& prog, const uint16_t vert_size, SQUE_VertAttrib* attr)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    attr->id = glGetAttribLocation(prog.id, attr->name);
    glEnableVertexAttribArray(attr->id);
    glVertexAttribPointer(attr->id, attr->num_comp,
        attr->var_type,
        attr->normalize,
        vert_size,
        (void*)attr->offset);
#endif
}

void SQUE_MESH_EnableAttribute(const uint16_t vert_size, const SQUE_VertAttrib& attr)
{

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glEnableVertexAttribArray(attr.id);
    glVertexAttribPointer(attr.id, 
                        attr.num_comp, 
                        attr.var_type, 
                        attr.normalize, 
                        vert_size, 
                        (void*)attr.offset);
#endif
}



void SQUE_RENDER_BindExternalTexture(int tex_type, uint32_t id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(tex_type, id);
#endif
}

void SQUE_RENDER_BindSampler(int32_t texture_locator, int32_t sampler_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    if(GetGLVer() >= 330)
        glBindSampler(texture_locator, sampler_id);
#elif defined(USE_OPENGLES)
    // ?
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING /////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_RENDER_DrawIndices(const SQUE_Mesh& mesh, const int32_t offset_indices, int32_t count)
{
    count = (count < 0) ? mesh.num_index : count;
    SQUE_MESH_BindBuffer(mesh);
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glDrawElements(mesh.draw_config, count, mesh.index_var, (void*)(mesh.index_var_size * offset_indices));
#else
#endif
}

void SQUE_RENDER_DrawVertices(const SQUE_Mesh& mesh, int32_t count)
{
    count = (count == 0) ? mesh.num_verts : count;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(mesh.attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_id);
    glDrawArrays(mesh.draw_config, 0, count);
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG... //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#   include <Windows.h>
#   define FOLDER_ENDING '\\'
#else
#   define FOLDER_ENDING '/'
#   ifdef ANDROID
#       include<android/log.h>
#   endif
#endif

#include <cstring>
void CheckForRenderErrors(const char* file, int line)
{
    int32_t errcode = 0;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    while ((errcode = glGetError()) != GL_NO_ERROR)
    {
        SQUE_PRINT(LT_WARNING, "%s(%d): OpenGL Error %d", strrchr(file, FOLDER_ENDING), line, errcode);
    }  
#endif
}

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
void glDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    SQUE_PRINT(LT_INFO, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}

void InitGLDebug()
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebug, 0);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, false);
}
#else
void InitGLDebug()
{}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FRAMEBUFFER ///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_FRAMEBUFFER_GenerateID(uint32_t* framebuffer_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenFramebuffers(1, framebuffer_id);
#endif
}

void SQUE_FRAMEBUFFER_Bind(const int32_t type, const uint32_t id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindFramebuffer(type, id);
#endif
}

void SQUE_FRAMEBUFFER_GenRenderTypeID(uint32_t* renderbuffer_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGL)
    glGenRenderbuffers(1, renderbuffer_id);    
#endif
}

void SQUE_FRAMEBUFFER_BindRenderType(const uint32_t renderbuffer_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_id);
#endif
}

void SQUE_FRAMEBUFFER_SetRenderTypeInfo(const uint32_t type, const uint32_t width, const uint32_t height)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glRenderbufferStorage(GL_RENDERBUFFER, type, width, height);
#endif
}

void SQUE_FRAMEBUFFER_AttachRenderType(const uint32_t attachment_type, const uint32_t attachment_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment_type, GL_RENDERBUFFER, attachment_id);
#endif
}

void SQUE_FRAMEBUFFER_AttachTexture(const uint32_t dest_attachment, const uint32_t texture_id, const uint32_t mipmap_level)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glFramebufferTexture(GL_FRAMEBUFFER, dest_attachment + GL_COLOR_ATTACHMENT0, texture_id, mipmap_level);
    glFramebufferTexture2D(GL_FRAMEBUFFER, dest_attachment + GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, mipmap_level);
#endif
}

void SQUE_FRAMEBUFFER_SetDrawBuffers(const uint32_t* attachments, const uint32_t size)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glDrawBuffers(size, attachments);
#endif
}

uint32_t SQUE_FRAMEBUFFER_CheckStatus()
{
    bool fb_status;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    return glCheckFramebufferStatus(GL_FRAMEBUFFER);
    //fb_status = (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE);

    GL_FRAMEBUFFER_COMPLETE;
    GL_FRAMEBUFFER_UNDEFINED; // 33305
    GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT; // 36054
    GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT; //+1
    GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER; //+2
    GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER; //36060
    GL_FRAMEBUFFER_UNSUPPORTED; //+1
        
#endif
    if(!fb_status) SQUE_CHECK_RENDER_ERRORS();
}

void SQUE_RENDER_FramebufferDelete(uint32_t framebuffer_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glDeleteFramebuffers(1, &framebuffer_id);
#endif
}

void SQUE_RENDER_RenderbufferDelete(uint32_t renderbuffer_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glDeleteRenderbuffers(1, &renderbuffer_id);
#endif
}
/*
void Test2()
{
    // Bind Framebuffer
    // call to Viewport to set where you are rendering
    // glViewport(0,0, textuerWidth, textureHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the texture because it was dirty last frame
    // glDrawIndices / glDrawArrays / ...

    // set to the required framebuffer again
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(...)
    glClear()...
    glBindTexture(GL_TEXTURE_2D, renderetTexture);
    glDraw(...) // Basically you can call setup/backup before doing render pipeline and on end render pipeline
    // thens(...)
     setup adn will render to expected!
    // You can generate mipmaps after if it is NEEDED!
    // glGenMipMap
    // could also just pick the previous...
    GLint prevFB;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevFB);
}
*/