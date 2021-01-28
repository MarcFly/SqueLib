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
    var_size(4), vert_size(0),
    offset(0), name("")
{}

SQUE_VertAttrib::SQUE_VertAttrib(const char* name_, int32_t var_type_, bool normalize_, uint16_t num_components) :
    id(0), var_type(var_type_), num_comp(num_components), 
    normalize(normalize_), vert_size(0), offset(0), name(name)
{   
    var_size = SQUE_VarGetSize(var_type);
}

uint16_t SQUE_VertAttrib::GetSize() const { return var_size * num_comp; }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// MESH MANAGEMENT ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CONSTRUCTORS / DESTRUCTORS ////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

SQUE_Mesh::SQUE_Mesh() : draw_config(SQUE_POINTS), draw_mode(SQUE_STATIC_DRAW),
    attribute_object(0), vert_id(0), num_verts(0), verts(NULL), index_id(0),
    num_index(0), index_var(SQUE_UINT), index_var_size(4), indices(NULL)
{}

SQUE_Mesh::SQUE_Mesh(int32_t draw_config_, int32_t draw_mode_, int32_t index_var_) :
    draw_config(draw_config_), draw_mode(draw_mode_),
    attribute_object(0), vert_id(0), num_verts(0),
    verts(NULL), index_id(0), num_index(0),
    index_var(index_var_), indices(NULL)
{   
    index_var_size = SQUE_VarGetSize(index_var);
}

SQUE_Mesh::~SQUE_Mesh() { if(vert_id > 0) CleanUp(); }

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHANGING DATA DYNAMICALLY /////////////////////////////////////////////////////////////////////////////
void SQUE_Mesh::ChangeVertData(int32_t num_verts_, void* verts_)
{
    num_verts = num_verts_;
    verts = verts_;
}

void SQUE_Mesh::ChangeIndexData(int32_t num_index_, void* indices_)
{
    num_index = num_index_;
    indices = indices_;
}

// LOCATIONS AND VERTEX ATTRIBUTES ///////////////////////////////////////////////////////////////////////
SQUE_VertAttrib* SQUE_Mesh::AddAttribute(SQUE_VertAttrib* attr)
{
    if (attr == NULL) attr = new SQUE_VertAttrib();
    attributes.push_back(attr);
    return attr;
}

void SQUE_Mesh::EnableAttributesForProgram(const SQUE_Program& program)
{
    uint16_t size = attributes.size();
    for (int i = 0; i < size; ++i)
        SQUE_EnableProgramAttribute(program, attributes[i]);
}

void SQUE_Mesh::SetLocationsInOrder()
{
    SetOffsetsInOrder();

    uint16_t size = attributes.size();
    // So not to bind the Element Array (which will test if causes errors)
    int32_t temp_id = index_id;
    index_id = 0;
    SQUE_BindMeshBuffer(*this);
    index_id = temp_id;

    uint16_t vert_size = GetVertSize();
    for (int i = 0; i < size; ++i)
        SQUE_EnableBufferAttribute(i, vert_size, attributes[i]);
}

void SQUE_Mesh::SetAttributeLocation(const char* name, const int32_t location)
{
    uint16_t size = attributes.size();
    uint16_t vert_size = GetVertSize();
    for (int i = 0; i < size; ++i)
        if( strcmp(attributes[i]->name, name) == 0) 
            attributes[i]->id = location;
}

// Getters ///////////////////////////////////////////////////////////////////////////////////////////////
uint16_t SQUE_Mesh::GetVertSize() const
{
    uint16_t ret = 0;
    int size = attributes.size();
    for (int i = 0; i < size; ++i)
        ret += attributes[i]->GetSize();

    return ret;
}

uint16_t SQUE_Mesh::GetAttribSize(const char* name) const
{
    int size = attributes.size();
    for (int i = 0; i < size; ++i)
        if(strcmp(attributes[i]->name, name) == 0)  
            return attributes[i]->GetSize();
    return 0;
}

void SQUE_Mesh::CleanUp()
{
    if (verts != NULL) { delete[] verts; verts = NULL; }
    if (indices != NULL) { delete[] indices; indices = NULL; }
    num_verts = 0;
    num_index = 0;
    SQUE_PRINT(LT_INFO, "Deleted Attributes from mesh...");
    for (int i = 0; i < attributes.size(); ++i)
        delete attributes[i];
    attributes.clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE USAGE FUNCTIONS ///////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void SQUE_Mesh::SetOffsetsInOrder()
{
    uint16_t size = attributes.size();
    uint32_t offset = 0;
    for (int i = 0; i < size; ++i)
    {
        attributes[i]->offset = offset;
        offset += attributes[i]->GetSize();
    }
}

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