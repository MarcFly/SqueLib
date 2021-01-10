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
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array_buffer);
    glBindVertexArray(attribute_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
    glBlendEquationSeparate(blend_equation_rgb, blend_equation_alpha);
    glBlendFuncSeparate(blend_func_src_rgb, blend_func_dst_rgb, blend_func_src_alpha, blend_func_dst_alpha);

    glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
    glScissor(scissor_box.x, scissor_box.y, scissor_box.z, scissor_box.w);
#ifdef GL_POLYGON_MODE
    glPolygonMode(polygon_mode.x, polygon_mode.y);
#endif
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
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &texture);
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
    glGetIntegerv(GL_POLYGON_MODE, &polygon_mode.y);
    polygon_mode.x = FLY_FRONT_AND_BACK;
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
// ATTRIBUTE MANAGEMENT //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

int VarGetSize(int type_macro)
{
    if (type_macro == FLY_BYTE || type_macro == FLY_UBYTE /*add 1byte types*/)
        return 1;
    else if (type_macro == FLY_DOUBLE/*add 8 byte types*/)
        return 8;
    else if (type_macro == FLY_FLOAT || type_macro == FLY_UINT || type_macro == FLY_INT/* add 4 byte types*/)
        return 4;
    else if (type_macro == FLY_USHORT)
        return 2;
    return 0;
}

void FLY_Attribute::SetName(const char* str) { name = str; }
void FLY_Attribute::SetVarType(int32 var) { var_type = var; var_size = VarGetSize(var_type); }
void FLY_Attribute::SetNumComponents(uint16 num) { num_comp = num; }
void FLY_Attribute::SetNormalize(bool norm) { normalize = norm; }
void FLY_Attribute::SetOffset(uint32 offset_bytes) { offset = offset_bytes; }
void FLY_Attribute::SetId(int32 id_) { id = id_; }
uint16 FLY_Attribute::GetSize() const { return num_comp*var_size; }

void FLY_Attribute::SetAttribute(uint16 vert_size) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glEnableVertexAttribArray(id);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
    glVertexAttribPointer(id, num_comp, var_type, normalize, vert_size, (void*)offset);
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
#endif
}

void FLY_Attribute::SetLocation(uint16 pos, uint16 vert_size)
{
    id = pos;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glVertexAttribPointer(pos, num_comp, var_type, normalize, vert_size, (void*)offset);
    glEnableVertexAttribArray(pos);
    
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initialization
void FLY_Mesh::SetDrawMode(int32 d_m) { draw_mode = d_m; }
void FLY_Mesh::SetIndexVarType(int32 var) { index_var = var; index_var_size = VarGetSize(index_var); };
void FLY_Mesh::GiveAttribute(FLY_Attribute** atr)
{
    if (atr == nullptr || *atr == nullptr)
    {
        FLYPRINT(LT_WARNING, "Non-existent attribute passed...");
        return;
    }
    attributes.push_back(*atr);
    *atr = nullptr;
}

void FLY_Mesh::SetOffsetsInOrder()
{
    uint16 size = attributes.size();
    uint32 offset = 0;
    for (int i = 0; i < size; ++i)
    {
        attributes[i]->SetOffset(offset);
        offset += attributes[i]->GetSize();
    }
}

void FLY_Mesh::Prepare()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glGenBuffers(1, &vert_id);
    glGenBuffers(1, &index_id);
    glGenVertexArrays(1, &attribute_object);
#endif
}

// Getters
uint16 FLY_Mesh::GetVertSize()
{
    uint16 size = 0;
    int attrs = attributes.size();
    for (int i = 0; i < attributes.size(); ++i)
        size += attributes[i]->GetSize();

    return size;
}

// Usage
void FLY_Mesh::Bind()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
    //glBindBuffer(GL_ARRAY_BUFFER, vert_id);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
#endif
}

void FLY_Mesh::SetAttributes()
{
    uint16 size = attributes.size();
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
#endif
    uint16 vert_size = GetVertSize();
    for (int i = 0; i < size; ++i)
    {
        FLY_Attribute* atr = attributes[i];
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
        glEnableVertexAttribArray(atr->id);
#endif
        atr->SetAttribute(vert_size);
    }
}

void FLY_Mesh::SetLocationsInOrder()
{
    uint16 size = attributes.size();
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, vert_id);
#endif
    uint16 vert_size = GetVertSize();
    for (int i = 0; i < size; ++i)
        attributes[i]->SetLocation(i, vert_size);
}



void FLY_Mesh::SendToGPU()
{
#if defined(USE_OPENGL)  || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, vert_id);
    int buffer_size = GetVertSize() * num_verts;
    glBufferData(GL_ARRAY_BUFFER, buffer_size, verts, draw_mode);
    if (num_index > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_index*VarGetSize(index_var), indices, draw_mode);
    }
#endif
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t);
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

void FLYRENDER_BindExternalTexture(int tex_type, uint32 id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(tex_type, id);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEBUG... //////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void GetGLError()
{
    FLYLOG(LT_WARNING, "OpenGL ERROR: %d", glGetError());
}