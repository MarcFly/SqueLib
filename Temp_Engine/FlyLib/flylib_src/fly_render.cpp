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
    FLYLOG(FLY_LogType::LT_INFO, "OpenGL Core Version: %d.%d", GLVersion.major, GLVersion.minor);
#elif defined USE_OPENGLES
    FLYLOG(FLY_LogType::LT_INFO, "OpenGLES Version: %d.%d", GLVersion.major, GLVersion.minor);
#endif
    // Generate Viewport with window size
    uint16 w, h;
    FLYDISPLAY_GetSize(0, &w, &h);
    FLYRENDER_ChangeViewPortSize(w, h);
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

#if defined(USE_OPENGL) ||defined(USE_OPENGLES)
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
    if (ver > 320)
#if defined(USE_OPENGLES)
        return "#version 320 es\nprecision mediump float;\n";
#else
        return "#version 330 core\n";
#endif
    else
        return "#version 100 es";
#endif

    return ret;
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

void FLY_VertAttrib::SetName(const char* str) { name = str; }
void FLY_VertAttrib::SetVarType(int32 var) { var_type = var; var_size = VarGetSize(var_type); }
void FLY_VertAttrib::SetNumComponents(uint16 num) { num_comp = num; }
void FLY_VertAttrib::SetNormalize(bool norm) { normalize = norm; }
void FLY_VertAttrib::SetOffset(uint32 offset_bytes) { offset = offset_bytes; }
void FLY_VertAttrib::SetId(int32 id_) { id = id_; }
uint16 FLY_VertAttrib::GetSize() const { return num_comp*var_size; }

void FLY_VertAttrib::EnableAsAttribute(int32 prog_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    id = glGetAttribLocation(prog_id, name);
    glEnableVertexAttribArray(id);
#endif
}

void FLY_VertAttrib::SetAttribute(uint16 vert_size) const
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, num_comp, var_type, normalize, vert_size, (void*)offset);
#endif
}

void FLY_VertAttrib::SetLocation(int32 pos, uint16 vert_size)
{
    id = pos;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glEnableVertexAttribArray(pos);
    glVertexAttribPointer(pos, num_comp, var_type, normalize, vert_size, (void*)offset);        
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initialization
void FLY_Mesh::SetDrawMode(int32 d_m) { draw_mode = d_m; }
void FLY_Mesh::SetIndexVarType(int32 var) { index_var = var; index_var_size = VarGetSize(index_var); };
FLY_VertAttrib* FLY_Mesh::AddAttribute(FLY_VertAttrib* atr)
{
    FLY_VertAttrib* ret = atr;
    if (atr == NULL) ret = new FLY_VertAttrib();
    attributes.push_back(ret);
    return ret;
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

void FLY_Mesh::EnableAttributesForProgram(int32 prog_id)
{
    uint16 size = attributes.size();
    for (int i = 0; i < size; ++i)
        attributes[i]->EnableAsAttribute(prog_id);
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
    glBindBuffer(GL_ARRAY_BUFFER, vert_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
#endif
}

void FLY_Mesh::BindNoIndices()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
    glBindBuffer(GL_ARRAY_BUFFER, vert_id);
#endif
}

void FLY_Mesh::SetAttributes()
{
    uint16 size = attributes.size();
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindVertexArray(attribute_object);
#endif
    Bind();
    uint16 vert_size = GetVertSize();
    for (int i = 0; i < size; ++i)
        attributes[i]->SetAttribute(vert_size);
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
    glBufferData(GL_ARRAY_BUFFER, buffer_size, (const void*)verts, draw_mode);
    if (num_index > 0)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_index*index_var_size, (const void*)indices, draw_mode);
    }
#endif
}

// CleanUp
FLY_Mesh::~FLY_Mesh() { CleanUp(); }
void FLY_Mesh::CleanUp()
{
    if (verts != NULL) { delete[num_verts*GetVertSize()] verts; verts = NULL; }
    if (indices != NULL) { delete[num_index*index_var_size] indices; indices = NULL; }
    for (int i = 0; i < attributes.size(); ++i)
        delete attributes[i];
    attributes.clear();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void FLY_TexAttrib::SetID(int32 id_) { id = id_; }
void FLY_TexAttrib::SetVarType(int32 var) { var_type = var; }
void FLY_TexAttrib::SetData(void* data_) { data = data_; }
void FLY_TexAttrib::Set(int32 id_, int32 var_, void* data_) { id = id_; var_type = var_, data = data_; }
void FLY_TexAttrib::SetParameter(int32 tex_id)
{
    if (tex_id < 1 || var_type == INT32_MAX || id == INT32_MAX || data == NULL)
        return;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    if (var_type == FLY_FLOAT)
        glTexParameterfv(FLY_TEXTURE_2D, id, (const float*)data);
    else
        glTexParameteriv(FLY_TEXTURE_2D, id, (const int32*)data);
#endif
    FLY_CHECK_RENDER_ERRORS();
}
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
FLY_TexAttrib* FLY_Texture2D::AddParameter(FLY_TexAttrib* tex_attrib)
{
    FLY_TexAttrib* ret = tex_attrib;
    if (ret == NULL) ret = new FLY_TexAttrib();
    attributes.push_back(ret);
    return ret;
}
void FLY_Texture2D::SetParameters()
{
    Bind();
    uint16 size = attributes.size();
    for (int i = 0; i < size; ++i)
        attributes[i]->SetParameter(id);
}

void FLY_Texture2D::Bind()
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(GL_TEXTURE_2D, id);
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
void FLYRENDER_ActiveTexture(int32 texture_id)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glActiveTexture(texture_id);
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

void CheckForRenderErrors(const char* file, int line)
{
    int32 errcode = 0;
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    while ((errcode = glGetError()) != 0)
        FLYLOG(LT_WARNING, "%s(%d): OpenGL Error %d", strrchr(file, FOLDER_ENDING), line, errcode);
#endif
}
