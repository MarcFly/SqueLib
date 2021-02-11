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
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// INITIALIZATION AND STATE CONTROL //////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

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
    // Generate Viewport with window size
    uint16_t w, h;
    /*SQUE_DISPLAY_MakeContextMain(0);
    SQUE_DISPLAY_GetWindowSize(0, &w, &h);
    SQUE_RENDER_ChangeViewPortSize(w, h);*/
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
const char* SQUE_RENDER_GetGLSLVer()
{
    const char* ret = "";
#if defined(USE_OPENGL) ||defined(USE_OPENGLES)
    char* str = NULL;
    int ver = GLVersion.major * 100 + GLVersion.minor * 10;
    int16_t size = 16;
    if (ver >= 320)
    {        
        const char* gles = "\n";
#if defined(USE_OPENGLES)
        size += 42;
        gles = " es\nprecision mediump float;\n";
#endif
        str = new char[size];
        sprintf(str, "#version %d %s", ver, gles);            
        ret = str;
    }
    else
        ret = "";
#endif
    SQUE_PRINT(LT_INFO, "%s", ret);
    return ret;
}

void SQUE_RENDER_Scissor(int x, int y, int w, int h)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glScissor(x, y, w, h);
#endif
}

void SQUE_RENDER_GetViewport(glm::vec4* vec)
{
    int vint[4];
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGetIntegerv(GL_VIEWPORT, vint);
#endif
    vec->x = vint[0];
    vec->y = vint[1];
    vec->z = vint[2];
    vec->w = vint[3];
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

void SQUE_GenerateMeshBuffer(SQUE_Mesh* mesh)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenBuffers(1, &mesh->vert_id);
    glGenBuffers(1, &mesh->index_id);
    glGenVertexArrays(1, &mesh->attribute_object);
#endif
}

void SQUE_BindMeshBuffer(const SQUE_Mesh& mesh)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(mesh.attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_id);
    if(mesh.num_index > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_id);
#endif
}

void SQUE_SendMeshToGPU(const SQUE_Mesh& mesh)
{
    int buffer_size = mesh.GetVertSize() * mesh.num_verts;
    SQUE_BindMeshBuffer(mesh);
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBufferData(GL_ARRAY_BUFFER, buffer_size, (const void*)mesh.verts, mesh.draw_mode);
    if (mesh.num_index > 0)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                    mesh.num_index * mesh.index_var_size, 
                    (const void*)mesh.indices, 
                    mesh.draw_mode);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// VERTEX ATTRIBUTE MANAGEMENT ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_EnableProgramAttribute(const SQUE_Program& prog,  SQUE_VertAttrib* attr)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    attr->id = glGetAttribLocation(prog.id, attr->name);
    glEnableVertexAttribArray(attr->id);
    glVertexAttribPointer(attr->id, attr->num_comp,
        attr->var_type,
        attr->normalize,
        attr->vert_size,
        (void*)attr->offset);
#endif
}

// For a location, set the ID to the location and SendToGPU
void SQUE_SendAttributeToGPU(const SQUE_VertAttrib& attr)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glEnableVertexAttribArray(attr.id);
    glVertexAttribPointer(attr.id, attr.num_comp, 
                        attr.var_type, 
                        attr.normalize, 
                        attr.vert_size, 
                        (void*)attr.offset);
#endif
}

void SQUE_EnableBufferAttribute(int32_t location, uint16_t vert_size, SQUE_VertAttrib* attr)
{
    attr->id = location;
    attr->vert_size = vert_size;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glEnableVertexAttribArray(attr->id);
    glVertexAttribPointer(attr->id, 
                        attr->num_comp, 
                        attr->var_type, 
                        attr->normalize, 
                        vert_size, 
                        (void*)attr->offset);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE ATTRIBUT MANAGEMENT ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_SetTextureParameters(const SQUE_Texture2D& tex, const SQUE_TexAttrib& attr)
{
    if (tex.id < 1 || attr.var_type < 0 || attr.id == UINT32_MAX || attr.data == NULL)
        return;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    if (attr.var_type == SQUE_FLOAT)
        glTexParameterfv(SQUE_TEXTURE_2D, attr.id, (const float*)attr.data);
    else
        glTexParameteriv(SQUE_TEXTURE_2D, attr.id, (const int32_t*)attr.data);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void SQUE_GenTextureData(SQUE_Texture2D* tex)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenTextures(1, &tex->id);
#endif
}

void SQUE_BindTexture(const SQUE_Texture2D& tex)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(GL_TEXTURE_2D, tex.id);
#endif
}

void SQUE_SetActiveTextureUnit(int32_t unit)
{
#if defined(USE_OPENGL) | defined(USE_OPENGLES)
    glActiveTexture(unit);
#endif
}

void SQUE_SendTextureToGPU(const SQUE_Texture2D& tex, int32_t mipmap_level)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(GL_TEXTURE_2D, tex.id);
    // TODO: read about texture border, uses...
    glTexImage2D(GL_TEXTURE_2D, mipmap_level, tex.format, tex.w, tex.h, 0, tex.format, tex.var_type, tex.pixels);
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
    if(GetGLVer() >= 330)glBindSampler(texture_locator, sampler_id);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADER MANAGEMENT /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// SHADER PROGRAM MANAGEMENT /////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_RENDER_CreateProgram(SQUE_Program* prog)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    prog->id = glCreateProgram();
#endif
}

void SQUE_RENDER_LinkProgram(const SQUE_Program& prog)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glLinkProgram(prog.id);
#endif
    SQUE_RENDER_CheckProgramLog(prog);
}

void SQUE_RENDER_UseProgram(const SQUE_Program& prog)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glUseProgram(prog.id);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING /////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_RENDER_DrawIndices(const SQUE_Mesh& mesh, int32_t offset_indices, int32_t count)
{
    count = (count < 0) ? mesh.num_index : count;
    SQUE_BindMeshBuffer(mesh);
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(mesh.attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_id);
    glDrawElements(mesh.draw_config, count, mesh.index_var, (void*)(mesh.index_var_size * offset_indices));
#else
#endif
}

void SQUE_RENDER_DrawVertices(const SQUE_Mesh& mesh, int32_t count)
{
    count = (count == 0) ? mesh.num_verts : count;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
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
        SQUE_PRINT(LT_WARNING, "%s(%d): OpenGL Error %d", strrchr(file, FOLDER_ENDING), line, errcode);
#endif
}
