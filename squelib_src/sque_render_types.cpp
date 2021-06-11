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
// VERTEX ATTRIBUTE MANAGEMENT ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
SQUE_VertAttrib::SQUE_VertAttrib() :
    id(0), var_type(SQUE_FLOAT),
    num_comp(0), normalize(false),
    var_size(4), offset(0)
{}

SQUE_VertAttrib::SQUE_VertAttrib(const char* name_, int32_t var_type_, bool normalize_, uint16_t num_components) :
    id(0), var_type(var_type_), num_comp(num_components), 
    normalize(normalize_), offset(0)
{   
    var_size = SQUE_VarGetSize(var_type);
    memcpy(name, name_, strlen(name_));
}

SQUE_VertAttrib::~SQUE_VertAttrib() {}

uint16_t SQUE_VERTEX_ATTRIBUTE_GetSize(uint16_t vertex_size, uint16_t num_components) { return num_components * vertex_size; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE ATTRIBUT MANAGEMENT ///////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_TEXTURE_ApplyAttributes(const SQUE_Texture& tex)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    for (uint32_t i = 0; i < tex.attributes.size(); ++i)
    {
        void* data = tex.attributes[i].data;
        if (tex.attributes[i].type == SQUE_INT)
            glTexParameteriv(tex.dim_format, tex.attributes[i].id, (const int32_t*)data);
        else if (tex.attributes[i].type == SQUE_FLOAT)
            glTexParameterfv(tex.dim_format, tex.attributes[i].id, (const float*)data);
    }
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void SQUE_TEXTURE_GenBufferIDs(const uint32_t num, uint32_t* tex_ids)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenTextures(num, tex_ids);
#endif
}

void SQUE_TEXTURE_GenMipmaps(const uint32_t texture_type)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glGenerateMipmap(texture_type);
#endif
}

void SQUE_TEXTURE_Bind(const uint32_t texture_id, const int32_t texture_dims)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    glBindTexture(texture_dims, texture_id);
#endif
}

void SQUE_TEXTURE_SetActiveUnit(int32_t unit)
{
#if defined(USE_OPENGL) | defined(USE_OPENGLES)
    glActiveTexture(unit);
#endif
}

// Why as 2D
// glTexImage1D and glTexImage3D are separate and use different types of texture inputs
// i will not deal with them
void SQUE_TEXTURE_SendAs2DToGPU(const SQUE_Texture& tex, void* pixels, int32_t mipmap_level)
{
#if defined(USE_OPENGL) || defined(USE_OPENGLES)
    // TODO: read about texture border, uses...
    glTexImage2D(tex.dim_format, mipmap_level, tex.use_format, tex.w, tex.h, 0, tex.data_format, tex.var_type, pixels);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_Mesh::SQUE_Mesh() : draw_config(SQUE_POINTS), draw_mode(SQUE_STATIC_DRAW),
    attribute_object(0), vert_id(0), num_verts(0), index_id(0),
    num_index(0), index_var(SQUE_UINT), index_var_size(4)
{}

SQUE_Mesh::~SQUE_Mesh() {};
//SQUE_Mesh::~SQUE_Mesh() { if(vert_id > 0) CleanUp(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHANGING DATA DYNAMICALLY /////////////////////////////////////////////////////////////////////////////
void SQUE_MESH_SetDrawConfig(SQUE_Mesh* mesh, const int32_t draw_config_, const int32_t draw_mode_)
{
    mesh->draw_config = draw_config_;
    mesh->draw_mode = draw_mode_;
}

void SQUE_MESH_SetDataConfig(SQUE_Mesh* mesh, const uint32_t num_verts_, const uint32_t num_index_, const uint32_t index_var_)
{
    mesh->num_verts = num_verts_;
    mesh->num_index = num_index_;
    mesh->index_var = index_var_;
    mesh->index_var_size = SQUE_VarGetSize(index_var_);
}

void SQUE_MESH_SetVertData(SQUE_Mesh* mesh, const uint32_t num_verts_)
{
    mesh->num_verts = num_verts_;
}

void SQUE_MESH_SetIndexData(SQUE_Mesh* mesh, const uint32_t num_index_, const uint32_t index_var_)
{
    mesh->num_index = num_index_;
    mesh->index_var = index_var_;
    mesh->index_var_size = SQUE_VarGetSize(index_var_);
}

void SQUE_MESH_AddAttribute(SQUE_Mesh* mesh, const char* name_, const int32_t var_type, const bool normalize, const uint16_t num_components)
{
    mesh->attributes.push_back(SQUE_VertAttrib(name_, var_type, normalize, num_components));
}

void SQUE_MESH_SetLocations(SQUE_Mesh* mesh)
{
    sque_vec<SQUE_VertAttrib>& v_attribs = mesh->attributes;

    SQUE_MESH_CalcVertSize(mesh);

    for (uint32_t i = 0; i < v_attribs.size(); ++i)
    {
        v_attribs[i].id = i;
        SQUE_MESH_EnableAttribute(mesh->vertex_size, v_attribs[i]);
    }
}

// Getters ///////////////////////////////////////////////////////////////////////////////////////////////
uint16_t SQUE_MESH_CalcVertSize(SQUE_Mesh* mesh)
{
    sque_vec<SQUE_VertAttrib>& v_attribs = mesh->attributes;

    mesh->vertex_size = 0;

    for (uint32_t i = 0; i < v_attribs.size(); ++i)
        mesh->vertex_size += v_attribs[i].var_size * v_attribs[i].num_comp;

    return mesh->vertex_size;
}

void SQUE_MESH_InterleaveOffsets(SQUE_Mesh* mesh)
{
    SQUE_MESH_CalcVertSize(mesh);
    sque_vec<SQUE_VertAttrib>& v_attribs = mesh->attributes;
    uint32_t offset = 0;
    for (uint16_t i = 0; i < v_attribs.size(); ++i)
    {
        v_attribs[i].offset = offset;
        offset += v_attribs[i].num_comp * v_attribs[i].var_size;
    }
}

void SQUE_MESH_BlockOffsets(SQUE_Mesh* mesh)
{
    SQUE_MESH_CalcVertSize(mesh);
    sque_vec<SQUE_VertAttrib>& v_attribs = mesh->attributes;
    SQUE_MESH_CalcVertSize(mesh);
    uint32_t offset = 0;
    for (uint16_t i = 0; i < v_attribs.size(); ++i)
    {
        v_attribs[i].offset = offset;
        offset += v_attribs[i].num_comp * v_attribs[i].var_size * mesh->num_verts;
    }
}

uint16_t SQUE_MESH_GetAttribSize(const SQUE_Mesh& mesh, const char* name)
{
    const sque_vec<SQUE_VertAttrib>& v_attribs = mesh.attributes;

    for (uint32_t i = 0; i < v_attribs.size(); ++i)
        if(strcmp(v_attribs[i].name, name) == 0)  
            return v_attribs[i].var_size * v_attribs[i].num_comp;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE ATTRIBUTES MANAGEMENT /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_TexAttrib::SQUE_TexAttrib()
{}

SQUE_TexAttrib::SQUE_TexAttrib(const char* _name, int32_t attrib_id, int32_t value)
{
    memcpy(name, _name, strlen(_name));
    type = SQUE_INT;
    id = attrib_id;
    data = (void*)new int(value);
}

SQUE_TexAttrib::SQUE_TexAttrib(const char* _name, int32_t attrib_id, float value)
{
    memcpy(name, _name, strlen(_name));
    type = SQUE_FLOAT;
    id = attrib_id;
    data = (void*)new float(value);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_TEXTURE_SetFormat(SQUE_Texture* texture, const int32_t dimentions_f, const int32_t use_f, const int32_t data_f, const int32_t var_type)
{
    texture->dim_format = dimentions_f;
    texture->use_format = use_f;
    texture->data_format = data_f;
    texture->var_type = var_type;
    texture->var_size = SQUE_VarGetSize(var_type);
}

void SQUE_TEXTURE_SetDimentions(SQUE_Texture* texture, const int32_t width, const int32_t height, const int32_t num_channels)
{
    texture->w = width;
    texture->h = height;
    texture->channel_num = num_channels;
}

void SQUE_TEXTURE_AddAttribute(SQUE_Texture* texture, const char* name, int32_t attrib_id, int32_t value)
{
    texture->attributes.push_back(SQUE_TexAttrib(name, attrib_id, value));
}
void SQUE_TEXTURE_AddAttribute(SQUE_Texture* texture, const char* name, int32_t attrib_id, float value)
{
    texture->attributes.push_back(SQUE_TexAttrib(name, attrib_id, value));
}

