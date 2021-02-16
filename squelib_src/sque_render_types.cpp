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
void SQUE_MESHES_SetDrawConfig(SQUE_Mesh& mesh, int32_t draw_config_, int32_t draw_mode_)
{
    mesh.draw_config = draw_config_;
    mesh.draw_mode = draw_mode_;
}
void SQUE_MESHES_SetVertData(SQUE_Mesh& mesh, uint32_t num_verts_)
{
    mesh.num_verts = num_verts_;
}

void SQUE_MESHES_SetIndexData(SQUE_Mesh& mesh, uint32_t num_index_, uint32_t index_var_)
{
    mesh.num_index = num_index_;
    mesh.index_var = index_var_;
    mesh.index_var_size = SQUE_VarGetSize(index_var_);

    // Send indices to GPU
}

// LOCATIONS AND VERTEX ATTRIBUTES ///////////////////////////////////////////////////////////////////////
std::vector<SQUE_VertAttrib> vertex_attributes;
std::vector<SQUE_VertAttribIndex> mesh_attributes_index;
int32_t last = 0;

void SQUE_MESHES_DeclareAttributes(const int32_t vert_id, int32_t& attrib_ref, uint32_t num_attribs)
{
    int32_t cap = mesh_attributes_index.size() - 1;
    if (cap < last) mesh_attributes_index.resize(cap + 1 + 50);

    SQUE_VertAttribIndex index;
    index.id = vert_id;
    vertex_attributes.resize(vertex_attributes.size() + num_attribs);
    index.start_attrib = vertex_attributes.size()-num_attribs;
    index.end_attrib = index.start_attrib + num_attribs - 1;
    index.last = 0;
    
    mesh_attributes_index[last] = index;
    attrib_ref = last;
    last++;
    
}

SQUE_VertAttrib* SQUE_MESHES_AddAttribute(const int32_t attrib_ref, SQUE_VertAttrib& attr)
{
    SQUE_VertAttribIndex& index = mesh_attributes_index[attrib_ref];
    //++index.last;
    return &(vertex_attributes[index.start_attrib + index.last++] = attr);
}

//void EnableAttributesForProgram(const SQUE_Program& program)
//{
//    uint16_t size = attributes.size();
//    for (int i = 0; i < size; ++i)
//        SQUE_EnableProgramAttribute(program, attributes[i]);
//}

void SQUE_MESHES_SetLocations(const int32_t vert_id, const int32_t ind_id, const int32_t attr_obj, const int32_t attrib_ref)
{
    SQUE_VertAttribIndex& index = mesh_attributes_index[attrib_ref];
    uint32_t last = index.start_attrib + index.last;

    uint16_t vert_size = 0;
    for (uint32_t i = index.start_attrib; i < last; ++i)
    {
        vertex_attributes[i].offset = vert_size;
        vert_size += vertex_attributes[i].var_size * vertex_attributes[i].num_comp;
    }
    index.vert_size = vert_size;

    SQUE_BindMeshBuffer(vert_id, ind_id, attr_obj);

    for (uint32_t i = index.start_attrib; i < last; ++i)
    {
        vertex_attributes[i].id = i-index.start_attrib;
        SQUE_EnableBufferAttribute(vert_size, vertex_attributes[i]);
    }
}

// Getters ///////////////////////////////////////////////////////////////////////////////////////////////
uint16_t SQUE_MESHES_CalcVertSize(const uint32_t attrib_ref)
{
    SQUE_VertAttribIndex& ind = mesh_attributes_index[attrib_ref];
    ind.vert_size = 0;
    uint32_t last = ind.start_attrib + ind.last;

    for (uint32_t i = ind.start_attrib; i < last; ++i)
        ind.vert_size += vertex_attributes[i].var_size * vertex_attributes[i].num_comp;

    return ind.vert_size;
}

uint16_t SQUE_MESHES_GetVertSize(const uint32_t attrib_ref)
{
    return mesh_attributes_index[attrib_ref].vert_size;
}

uint16_t SQUE_MESHES_GetAttribSize(const uint32_t attrib_ref, const char* name)
{
    SQUE_VertAttribIndex& ind = mesh_attributes_index[attrib_ref];
    
    uint32_t last = ind.start_attrib + ind.last;

    for (uint32_t i = ind.start_attrib; i < last; ++i)
        if(strcmp(vertex_attributes[i].name, name) == 0)  
            return vertex_attributes[i].var_size * vertex_attributes[i].num_comp;
    return 0;
}

// TODO - Both here and uniforms, functions to move out/free memory from the vectors

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE ATTRIBUTES MANAGEMENT /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_TexAttrib::SQUE_TexAttrib() : id(0),   var_type(0),    data(NULL)
{}

SQUE_TexAttrib::SQUE_TexAttrib(int32_t parameter_id, int32_t var_type_, void* data_) :
    id(parameter_id),   var_type(var_type_),    data(data_)
{}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// TEXTURE MANAGEMENT ////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_Texture2D::SQUE_Texture2D() : id(UINT32_MAX),    format(0),  var_type(SQUE_FLOAT),    var_size(4),
    w(0),   h(0),   channel_num(0),  pixels(NULL)
{}

SQUE_Texture2D::SQUE_Texture2D(int32_t format_, int32_t var_type_) : id(UINT32_MAX),    format(format_),  
    var_type(var_type_),    w(0),   h(0),   channel_num(0),  pixels(NULL)
{
    var_size = SQUE_VarGetSize(var_type);
}
SQUE_Texture2D::~SQUE_Texture2D() {CleanUp();}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_Texture2D::SetParameter(SQUE_TexAttrib* attr)
{
    if (attr == NULL) return;
    uint16_t size = attributes.size();
    for(int i = 0; i < size; ++i)
    {
        if (attributes[i]->id == attr->id)
        {
            delete attributes[i];
            attributes[i] = attr;
            return;
        }
    }
    attributes.push_back(attr);
}

void SQUE_Texture2D::ApplyParameters()
{
    SQUE_BindTexture(*this);
    uint16_t size = attributes.size();
    for(int i = 0; i < size; ++i)
        SQUE_SetTextureParameters(*this, *attributes[i]);
}

void SQUE_Texture2D::CleanUp()
{
    // TODO: Texture CleanUp
}