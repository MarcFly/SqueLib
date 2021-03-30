#ifdef RENDER_SOLO
#   include "sque_render.h"
#else
#   include "squelib.h"
#endif
#include <cstring>

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

uint16_t SQUE_VERTEX_ATTRIBUTE_GetSize(uint16_t vertex_size, uint16_t num_components) { return num_components * vertex_size; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_Mesh::SQUE_Mesh() : draw_config(SQUE_POINTS), draw_mode(SQUE_STATIC_DRAW),
    attribute_object(0), vert_id(0), num_verts(0), index_id(0),
    num_index(0), index_var(SQUE_UINT), index_var_size(4)
{}

//SQUE_Mesh::~SQUE_Mesh() { if(vert_id > 0) CleanUp(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHANGING DATA DYNAMICALLY /////////////////////////////////////////////////////////////////////////////
void SQUE_MESH_SetDrawConfig(SQUE_Mesh& mesh, int32_t draw_config_, int32_t draw_mode_)
{
    mesh.draw_config = draw_config_;
    mesh.draw_mode = draw_mode_;
}
void SQUE_MESH_SetVertData(SQUE_Mesh& mesh, uint32_t num_verts_)
{
    mesh.num_verts = num_verts_;
}

void SQUE_MESH_SetIndexData(SQUE_Mesh& mesh, uint32_t num_index_, uint32_t index_var_)
{
    mesh.num_index = num_index_;
    mesh.index_var = index_var_;
    mesh.index_var_size = SQUE_VarGetSize(index_var_);

    // Send indices to GPU
}

// LOCATIONS AND VERTEX ATTRIBUTES ///////////////////////////////////////////////////////////////////////
sque_vec<SQUE_VertAttrib> vertex_attributes;
sque_vec<SQUE_VertAttribIndex> mesh_attributes_index;

void SQUE_MESH_DeclareAttributes(const int32_t vert_id, const uint16_t num_attribs, int32_t& attrib_ref)
{
    SQUE_VertAttribIndex index;
    index.id = vert_id;
    vertex_attributes.resize(vertex_attributes.size() + num_attribs);
    index.start_attrib = vertex_attributes.size()-num_attribs;
    index.end_attrib = index.start_attrib + num_attribs - 1;
    index.last = 0;
    
    mesh_attributes_index.push_back(index);
    attrib_ref = mesh_attributes_index.size();
}

SQUE_VertAttrib* SQUE_MESH_AddAttribute(const int32_t attrib_ref, SQUE_VertAttrib& attr)
{
    SQ_ASSERT(attrib_ref <= mesh_attributes_index.size() && "Texture not declared to have attributes");
    SQUE_VertAttribIndex& index = mesh_attributes_index[attrib_ref-1];
    SQ_ASSERT(index.end_attrib >= index.start_attrib + index.last && "Adding more texture attributes than declared initially");
    return &(vertex_attributes[index.start_attrib + index.last++] = attr);
}

void SQUE_MESH_SetLocations(const int32_t attrib_ref)
{
    SQUE_VertAttribIndex& index = mesh_attributes_index[attrib_ref-1];
    uint32_t last = index.start_attrib + index.last;

    uint16_t vert_size = 0;
    for (uint32_t i = index.start_attrib; i < last; ++i)
    {
        vertex_attributes[i].offset = vert_size;
        vert_size += vertex_attributes[i].var_size * vertex_attributes[i].num_comp;
    }
    index.vert_size = vert_size;

    for (uint32_t i = index.start_attrib; i < last; ++i)
    {
        vertex_attributes[i].id = i-index.start_attrib;
        SQUE_MESH_EnableAttribute(vert_size, vertex_attributes[i]);
    }
}

// Getters ///////////////////////////////////////////////////////////////////////////////////////////////
uint16_t SQUE_MESH_CalcVertSize(const uint32_t attrib_ref)
{
    SQUE_VertAttribIndex& ind = mesh_attributes_index[attrib_ref-1];
    ind.vert_size = 0;
    uint32_t last = ind.start_attrib + ind.last;

    for (uint32_t i = ind.start_attrib; i < last; ++i)
        ind.vert_size += vertex_attributes[i].var_size * vertex_attributes[i].num_comp;

    return ind.vert_size;
}

uint16_t SQUE_MESH_GetVertSize(const uint32_t attrib_ref)
{
    return mesh_attributes_index[attrib_ref-1].vert_size;
}

uint16_t SQUE_MESH_GetAttribSize(const uint32_t attrib_ref, const char* name)
{
    SQUE_VertAttribIndex& ind = mesh_attributes_index[attrib_ref];
    
    uint32_t last = ind.start_attrib + ind.last;

    for (uint32_t i = ind.start_attrib; i < last; ++i)
        if(strcmp(vertex_attributes[i].name, name) == 0)  
            return vertex_attributes[i].var_size * vertex_attributes[i].num_comp;
    return 0;
}

// TODO - Reference based Templated Dynamic Array

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE ATTRIBUTES MANAGEMENT /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_TexAttrib* SQUE_TEXTURE_GenAttrib(const int32_t attribute_id, const void* data_, const uint16_t data_size)
{
    SQUE_TexAttrib* tex_attrib = new SQUE_TexAttrib();
    tex_attrib->id = attribute_id;
    tex_attrib->data = malloc(data_size);
    memcpy(tex_attrib->data, data_, data_size);

    return tex_attrib;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_TEXTURE_SetFormat(SQUE_Texture* texture, const int32_t dimentions_format, const int32_t use_f, const int32_t data_f, const int32_t var_type)
{
    texture->dim_format = dimentions_format;
    texture->use_format = use_f;
    texture->data_format = data_f;
    texture->var_type = var_type;
    texture->var_size = SQUE_VarGetSize(var_type);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
sque_vec<SQUE_TexAttrib> int_attributes;
sque_vec<SQUE_TexAttrib> float_attributes;
sque_vec<SQUE_TexAttribIndex> tex_attributes_index;

void SQUE_TEXTURE_DeclareAttributes(const uint32_t tex_id, const uint16_t num_ints, const uint16_t num_floats, uint32_t* attrib_ref)
{
    SQUE_TexAttribIndex t;
    t.id = tex_id;
    
    t.int_start = int_attributes.size();
    t.int_end = t.int_start + num_ints - 1;
    t.int_last = 0;
    t.float_start = float_attributes.size();
    t.float_end = t.float_start + num_floats - 1;
    t.float_last = 0;
    int_attributes.resize(t.int_start + num_ints);
    float_attributes.resize(t.float_start + num_floats);

    tex_attributes_index.push_back(t);
    *attrib_ref = tex_attributes_index.size();
}

SQUE_TexAttrib* SQUE_TEXTURE_AddInt(const uint32_t attrib_ref, const int32_t attribute_id, const int32_t value)
{
    SQ_ASSERT(attrib_ref <= tex_attributes_index.size() && "Texture not declare to have attributes");
    SQUE_TexAttribIndex& t = tex_attributes_index[attrib_ref - 1];
    SQ_ASSERT(t.int_end >= t.int_start + t.int_last && "Adding more INT attributes than declared");
    SQUE_TexAttrib& t_a = int_attributes[t.int_start + t.int_last];
    t_a.id = attribute_id;
    t_a.data = malloc(sizeof(int32_t));
    memcpy(t_a.data, &value, sizeof(int32_t));
    return &int_attributes[t.int_start + t.int_last++ - 1];
}

SQUE_TexAttrib* SQUE_TEXTURE_AddIntVs(const uint32_t attrib_ref, const int32_t attribute_id, void* data_, const uint16_t num_ints)
{
    SQ_ASSERT(attrib_ref < tex_attributes_index.size() && "Texture not declare to have attributes");
    SQUE_TexAttribIndex& t = tex_attributes_index[attrib_ref -1];
    SQ_ASSERT(t.int_end <= t.int_start + t.int_last && "Adding more INT attributes than declared");
    SQUE_TexAttrib& t_a = int_attributes[t.int_start + t.int_last];
    t_a.data = malloc(sizeof(int32_t)*num_ints);
    memcpy(t_a.data, data_, sizeof(int32_t)*num_ints);
    return &int_attributes[t.int_start + t.int_last++ - 1];
}

SQUE_TexAttrib* SQUE_TEXTURE_AddFloat(const uint32_t attrib_ref, const int32_t attribute_id, const float value)
{
    SQ_ASSERT(attrib_ref < tex_attributes_index.size() && "Texture not declare to have attributes");
    SQUE_TexAttribIndex& t = tex_attributes_index[attrib_ref - 1];
    SQ_ASSERT(t.float_end <= t.float_start + t.float_last && "Adding more FLOAT attributes than declared");
    SQUE_TexAttrib& t_a = float_attributes[t.float_start + t.float_last];
    t_a.data = malloc(sizeof(float));
    memcpy(t_a.data, &value, sizeof(float));
    return &float_attributes[t.float_start + t.float_last++ - 1];
}

SQUE_TexAttrib* SQUE_TEXTURE_AddFloatVs(const uint32_t attrib_ref, const int32_t attribute_id, void* data_, const uint16_t num_floats)
{
    SQ_ASSERT(attrib_ref < tex_attributes_index.size() && "Texture not declare to have attributes");
    SQUE_TexAttribIndex& t = tex_attributes_index[attrib_ref - 1];
    SQ_ASSERT(t.float_end <= t.float_start + t.float_last && "Adding more FLOAT attributes than declared");
    SQUE_TexAttrib& t_a = float_attributes[t.float_start + t.float_last];
    t_a.data = malloc(sizeof(float) * num_floats);
    memcpy(t_a.data, data_, sizeof(float) * num_floats);
    return &float_attributes[t.float_start + t.float_last++ - 1];
}

void SQUE_TEXTURE_FreeAttributes()
{
    for (uint16_t i = 0; i < int_attributes.size(); ++i)
        free(int_attributes[i].data);
    for (uint16_t i = 0; i < float_attributes.size(); ++i)
        free(float_attributes[i].data);

    // Make sure these vectors don't hold the data and create a memory leak, have to test
}