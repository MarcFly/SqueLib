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

void FLY_RenderState::SetUp()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glUseProgram(program);
    glBindTexture(GL_TEXTURE_2D, bound_texture);
    glActiveTexture(active_texture_unit);

    glBindVertexArray(attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);

    glBlendEquationSeparate(blend_equation_rgb, blend_equation_alpha);
    if (blend_func_separate)
        glBlendFuncSeparate(blend_func_src_rgb, blend_func_dst_rgb, blend_func_src_alpha, blend_func_dst_alpha);
    else
        glBlendFunc(blend_func_src_alpha, blend_func_dst_alpha);
    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    glScissor(scissor_box.x, scissor_box.y, scissor_box.z, scissor_box.w);

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
    glPolygonMode(FLY_FRONT_AND_BACK, (GLenum)polygon_mode.x);
#   endif
#endif

    FLY_CHECK_RENDER_ERRORS();
}

void FLY_RenderState::BackUp()
{
    backed_up = true;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &bound_texture);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &active_texture_unit);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vertex_array_buffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &element_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &attribute_object);

    glGetIntegerv(GL_BLEND_EQUATION_RGB, &blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blend_equation_alpha);

    glGetIntegerv(GL_BLEND_SRC_RGB, &blend_func_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &blend_func_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_func_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_func_dst_alpha);

    glGetIntegerv(GL_VIEWPORT, &viewport.x);
    glGetIntegerv(GL_SCISSOR_BOX, &scissor_box.x);

    blend = glIsEnabled(GL_BLEND);
    cull_faces = glIsEnabled(GL_CULL_FACE);
    depth_test = glIsEnabled(GL_DEPTH_TEST);
    scissor_test = glIsEnabled(GL_SCISSOR_TEST);

    // OpenGL Version Specific Code
#   ifdef USE_OPENGL
    glGetIntegerv(GL_POLYGON_MODE, &polygon_mode.x);
#   endif

#endif

    FLY_CHECK_RENDER_ERRORS();
}

void FLYRENDER_ChangeViewPortSize(int width, int height)
{
#if defined(USE_OPENGL) || defined (USE_OPENGLES)
    glViewport(0, 0, width, height);
#endif
}

#ifdef USE_GLFW
#   include <GLFW/glfw3.h>
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    FLYRENDER_ChangeViewPortSize(width, height);
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
    FLYLOG(FLY_LogType::LT_INFO, "OpenGL Core Version: %d.%d", GLVersion.major, GLVersion.minor);
#elif defined USE_OPENGLES
    FLYLOG(FLY_LogType::LT_INFO, "OpenGLES Version: %d.%d", GLVersion.major, GLVersion.minor);
#endif
    // Generate Viewport with window size
    uint16 w, h;
    FLYDISPLAY_GetWindowSize(0, &w, &h);
    FLYRENDER_ChangeViewPortSize(w, h);
    FLYLOG(FLY_LogType::LT_INFO, "Main Viewport init...");
    return ret;
}

void FLYRENDER_Close()
{

}

void FLYRENDER_Clear(const ColorRGBA& color, int clear_flags)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
}

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
int32 GetGLVer()
{
    return GLVersion.major * 100 + GLVersion.minor * 10;
}
#endif

const char* FLYRENDER_GetGLSLVer()
{
    const char* ret = "";
#if defined(USE_OPENGL) ||defined(USE_OPENGLES)
    int ver = GLVersion.major * 100 + GLVersion.minor * 10;
    if (ver >= 320)
#if defined(USE_OPENGLES)
        ret = "#version 320 es\nprecision mediump float;\n";
#else
        ret = "#version 330 core\n";
#endif
    else
        ret = "#version 100";
#endif
    FLYPRINT(LT_INFO, "%s", ret);
    return ret;
}

void FLYRENDER_Scissor(int x, int y, int w, int h)
{
#if defined(USE_OPENL) || defined(USE_OPENGLES)
    glScissor(x, y, w, h);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DATA MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLY_GenerateMeshBuffer(FLY_Mesh* mesh)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenBuffers(1, &mesh->vert_id);
    glGenBuffers(1, &mesh->index_id);
    glGenVertexArrays(1, &mesh->attribute_object);
#endif
}

void FLY_BindMeshBuffer(const FLY_Mesh& mesh)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(mesh.attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vert_id);
    if(mesh.num_index > 0) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_id);
#endif
}

void FLY_SendMeshToGPU(const FLY_Mesh& mesh)
{
    int buffer_size = mesh.GetVertSize() * mesh.num_verts;
    FLY_BindMeshBuffer(mesh);
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

void FLY_EnableProgramAttribute(const FLY_Program& prog,  FLY_VertAttrib* attr)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    attr->id = glGetAttribLocation(prog.id, attr->name);
    glEnableVertexAttribArray(attr->id);
#endif
}

// For a location, set the ID to the location and SendToGPU
void FLY_SendAttributeToGPU(const FLY_VertAttrib& attr)
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

void FLY_EnableBufferAttribute(int32 location, uint16 vert_size, FLY_VertAttrib* attr)
{
    attr->id = location;
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

void FLY_SetTextureParameters(const FLY_Texture2D& tex, const FLY_TexAttrib& attr)
{
    if (tex.id < 1 || attr.var_type < 0 || attr.id == UINT32_MAX || attr.data == NULL)
        return;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    if (attr.var_type == FLY_FLOAT)
        glTexParameterfv(FLY_TEXTURE_2D, attr.id, (const float*)attr.data);
    else
        glTexParameteriv(FLY_TEXTURE_2D, attr.id, (const int32*)attr.data);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void FLY_GenTextureData(FLY_Texture2D* tex)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenTextures(1, &tex->id);
#endif
}

void FLY_BindTexture(const FLY_Texture2D& tex)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(GL_TEXTURE_2D, tex.id);
#endif
}

void FLY_SetActiveTextureUnit(int32 unit)
{
#if defined(USE_OPENGL) | defined(USE_OPENGLES)
    glActiveTexture(unit);
#endif
}

void FLY_SendTextureToGPU(const FLY_Texture2D& tex, int32 mipmap_level)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(GL_TEXTURE_2D, tex.id);
    // TODO: read about texture border, uses...
    glTexImage2D(GL_TEXTURE_2D, mipmap_level, tex.format, tex.w, tex.h, 0, tex.format, tex.var_type, tex.pixels);
#endif
}

void FLYRENDER_BindExternalTexture(int tex_type, uint32 id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(tex_type, id);
#endif
}

void FLYRENDER_BindSampler(int32 texture_locator, int32 sampler_id)
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

void FLYRENDER_CreateProgram(FLY_Program* prog)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    prog->id = glCreateProgram();
#endif
}

void FLYRENDER_LinkProgram(const FLY_Program& prog)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glLinkProgram(prog.id);
#endif
    FLYRENDER_CheckProgramLog(prog);
}

void FLYRENDER_UseProgram(const FLY_Program& prog)
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glUseProgram(prog.id);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// RENDERING /////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLYRENDER_DrawIndices(const FLY_Mesh& mesh, int32 offset_indices, int32 count)
{
    count = (count < 0) ? mesh.num_index : count;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(mesh.attribute_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.index_id);
    glDrawElements(mesh.draw_config, count, mesh.index_var, (void*)(mesh.index_var_size * offset_indices));
#else
#endif
}

void FLYRENDER_DrawVertices(const FLY_Mesh& mesh, int32 count)
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
    int32 errcode = 0;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    while ((errcode = glGetError()) != 0)
        FLYLOG(LT_WARNING, "%s(%d): OpenGL Error %d", strrchr(file, FOLDER_ENDING), line, errcode);
#endif
}
