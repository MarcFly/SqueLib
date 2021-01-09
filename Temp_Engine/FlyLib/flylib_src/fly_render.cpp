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
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);

    glBlendEquationSeparate(blend_equation_rgb, blend_equation_alpha);
    glBlendFuncSeparate(blend_func_src_rgb, blend_func_dst_rgb, blend_func_src_alpha, blend_func_dst_alpha);
    
    glViewport(last_vp[0], last_vp[1], last_vp[2], last_vp[3]);

    if (blend) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    if (cull_faces) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    if (depth_test) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
    if (scissor_test) glEnable(GL_SCISSOR_TEST);
    else glDisable(GL_SCISSOR_TEST);
#endif
}

void FLY_RenderState::BackUp()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);

    glGetIntegerv(GL_BLEND_EQUATION_RGB, &blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &blend_equation_alpha);

    glGetIntegerv(GL_BLEND_SRC_RGB, &blend_func_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, &blend_func_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &blend_func_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, &blend_func_dst_alpha);

    glGetIntegerv(GL_VIEWPORT, last_vp);

    blend = glIsEnabled(GL_BLEND);
    cull_faces = glIsEnabled(GL_CULL_FACE);
    depth_test = glIsEnabled(GL_DEPTH_TEST);
    scissor_test = glIsEnabled(GL_SCISSOR_TEST);
#endif
}

void FLYRENDER_ChangeViewPortSize(int width, int height)
{
#if defined(USE_OPENGL) || (USE_OPENGLES)
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
#if defined(USE_OPENGL)
    return "#version 330\n";
#else if defined(USE_OPENGLES)
    return "#version 320 es\nprecision mediump float;"
#endif
        return "#Please update this function properly, saving opengl versions and such";
}

void FLYRENDER_Scissor(int x, int y, int w, int h)
{
#if defined(USE_OPENL) || defined(USE_OPENGLES)
    glScissor(x, y, w, h);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// BUFFER MANAGEMENT /////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int GetSize(int type_macro)
{
    if (type_macro == FLY_BYTE || type_macro == FLY_UBYTE /*add 1byte types*/)
        return 1;
    else if (type_macro == FLY_DOUBLE/*add 8 byte types*/)
        return 8;
    else if (type_macro == FLY_FLOAT/* add 4 byte types*/)
        return 4;
    return 0;
}

void FLY_Buffer::SetVarTypes(int32 p, int32 c, int32 uv, int32 n)
{
    pos_var = p;
    color_var = c;
    uv_var = uv;
    normal_var = n;
    // tangent, bitangent,... whtever needed
    
    pos_size = GetSize(pos_var);
    color_size = GetSize(color_var);
    uv_size = GetSize(uv_var);
    normal_size = GetSize(normal_var);
}

void FLY_Buffer::SetIndexVarType(int32 ind)
{
    index_var = ind;
}

void FLY_Buffer::SetComponentSize(uint16 p, uint16 c, uint16 uv, uint16 n)
{
    pos_comp = p;
    color_comp = c;
    uv_comp = uv;
    normal_comp = n;
}

void FLY_Buffer::SetToNormalize(bool p, bool c, bool uv, bool n)
{
    pos_norm = p;
    color_norm = c;
    uv_norm = uv;
    normal_norm = n;
}

void FLY_Buffer::SetDrawMode(int32 d_m)
{
    draw_mode = d_m;
}

uint16 FLY_Buffer::GetPosSize() const { return pos_size * pos_comp; };
uint16 FLY_Buffer::GetColorSize() const { return color_size * color_comp; };
uint16 FLY_Buffer::GetUVSize() const { return uv_size * uv_comp; };
uint16 FLY_Buffer::GetNormalSize() const { return normal_size * normal_comp; };
uint16 FLY_Buffer::GetVertSize() const
{
    uint16 p_s = pos_size * pos_comp;
    uint16 c_s = color_size * color_comp;
    uint16 uv_s = uv_size * uv_comp;
    uint16 n_s = normal_size * normal_comp;
    return p_s + c_s + uv_s + n_s /* tangent, bitangent,... whatever needed*/;
}
// Has Errors
void FLY_Buffer::SetAttributes()
{
    if (glGetError() > 0)
    {
        bool break_here = 0;
    }
    int attribs = 0;

    int vert_size = GetVertSize();
    int offset = 0;
    
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
    // Vertex Position
    int size;
    if ((size = GetPosSize()) > 0)
    {
        glEnableVertexAttribArray(attribs);
        glVertexAttribPointer(attribs, pos_comp, pos_var, pos_norm, vert_size, (void*)(offset));
        offset += size;
    }attribs++;
    // Vertex Color
    if ((size = GetColorSize()) > 0)
    {
        glEnableVertexAttribArray(attribs);
        glVertexAttribPointer(attribs, color_comp, color_var, color_norm, vert_size, (void*)(offset));
        offset += size;
    }attribs++;
    // Vertex UV
    if ((size = GetUVSize()) > 0)
    {
        glEnableVertexAttribArray(attribs);
        glVertexAttribPointer(attribs, uv_comp, uv_var, uv_norm, vert_size, (void*)(offset));
        offset += size;
    }attribs++;
    // Vertex Normals
    if ((size = GetNormalSize()) > 0)
    {
        glEnableVertexAttribArray(attribs);
        glVertexAttribPointer(attribs, normal_comp, normal_var, normal_norm, vert_size, (void*)(offset));
        offset += size;
    }attribs++;
    // Other required data...
    glBindVertexArray(0);
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
        int buffer_size = buf->GetVertSize() * buf->num_verts;
        glBufferData(GL_ARRAY_BUFFER, buffer_size, buf->verts, buf->draw_mode);
        if (CHK_FLAG(buf->layout, FLYSHADER_LAYOUT_HAS_INDICES) || buf->num_index > 0)
        {
            glGenBuffers(1, &buf->index_id);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->index_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32) * buf->num_index, buf->indices, buf->draw_mode);
        }

#endif
        buffers[i]->SetAttributes();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLY_Texture2D::Init(int32 tex_format)
{
    format = tex_format;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenTextures(1, &id);
#endif
}

// THIS HAS TO BE REVISED
// Probably generating Macros depending on API WILL BE WAAAY Better but more cumbersome
// example, generate a fly_rendermacros.h and add there the macros separately, just include if necessary
// INSTEAD of using enums and switches...

void FLY_Texture2D::SetFiltering(int32 min, int32 mag, int32 wraps, int32 wrapt)
{
    min_filter = min;
    mag_filter = mag;
    wrap_s = wraps;
    wrap_t = wrapt;

#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
#endif
}

void FLY_Texture2D::SendToGPU()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    // DataType will be revised if needed, UBYTE seems quite standard...
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, pixels);
#endif
}

void FLYRENDER_BindExternalTexture(int tex_type, int32 id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(tex_type, id);
#endif
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