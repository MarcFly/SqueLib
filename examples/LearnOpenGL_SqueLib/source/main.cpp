#include <iostream>
#include <squelib.h>

enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

#include <cmath>

bool on_background = false;
void OnResume()
{
    on_background = false;
}
void OnGoBackground()
{
    on_background = true;
}

char* easy_concat(const char* s1, const char* s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char* result = new char[len1 + len2 + 1]; // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2); // +1 to copy the null-terminator
    memcpy(result + len1 + len2, "\0", 1);
    return result;
}

char* concat_len(const char* s1, int l1, const char* s2, int l2)
{
    char* result = new char[l1 + l2+1]; // Please check when doing [] or ()...
    memcpy(result, s1, l1);
    memcpy(result + l1, s2, l2); // +1 to copy the null-terminator
    result[l1 + l2] = '\0';
    //memcpy(result + l1 + l2, "\0", 1);
    return result;
}

const char* glsl_ver = NULL;

// LearnOpenGL_1_HelloTriangle
static float vertices1[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

static bool loaded_ch1 = false; 
SQUE_Mesh triangle1;
SQUE_Asset* vert_s1_file = NULL;
SQUE_Asset* frag_s1_file = NULL;
SQUE_Program triangle_program1;
SQUE_Shader vert_s1;
SQUE_Shader frag_s1;


void LearnOpenGL_1_Vertices()
{
    if(!loaded_ch1)
    {
        SQUE_MESHES_SetDrawConfig(triangle1, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESHES_SetVertData(triangle1, 3);
        SQUE_RENDER_GenerateMeshBuffer(&triangle1.vert_id, &triangle1.index_id, &triangle1.attribute_object);
        SQUE_RENDER_BindMeshBuffer(triangle1.vert_id, triangle1.index_id, triangle1.attribute_object);
        SQUE_MESHES_DeclareAttributes(triangle1.vert_id, triangle1.attrib_ref, 1);
        SQUE_VertAttrib aPos("aPos", SQUE_FLOAT, false, 3);
        SQUE_MESHES_AddAttribute(triangle1.attrib_ref, aPos);        

        SQUE_MESHES_CalcVertSize(triangle1.attrib_ref);
        SQUE_MESHES_SetLocations(triangle1.attrib_ref);
        SQUE_RENDER_SendMeshToGPU(triangle1, vertices1);

        vert_s1_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/vert_s1.vert");
        frag_s1_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/frag_s1.frag");
        std::string vert_source(concat_len(glsl_ver, strlen(glsl_ver), vert_s1_file->raw_data, vert_s1_file->size));
        std::string frag_source(concat_len(glsl_ver, strlen(glsl_ver), frag_s1_file->raw_data, frag_s1_file->size));

        SQUE_SHADERS_Generate(vert_s1, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_s1.id, vert_source.c_str());
        SQUE_SHADERS_Compile(vert_s1.id);

        SQUE_SHADERS_Generate(frag_s1, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_s1.id, frag_source.c_str());
        SQUE_SHADERS_Compile(frag_s1.id);

        // Have to Change names so that mesh, is for mesh, program for program,...
        // Consistency so that things make sense and are easy to find!
        SQUE_RENDER_CreateProgram(&triangle_program1.id);
        // Type in attaching could be avoided by passing the whole shader directly, it only holds type and id and needs both
        SQUE_PROGRAM_AttachShader(triangle_program1, vert_s1.id, vert_s1.type);
        SQUE_PROGRAM_AttachShader(triangle_program1, frag_s1.id, frag_s1.type);
        SQUE_RENDER_LinkProgram(triangle_program1.id);

        SQUE_SHADERS_FreeFromGPU(vert_s1.id);
        SQUE_SHADERS_FreeFromGPU(frag_s1.id);

        delete vert_s1_file->raw_data;
        delete vert_s1_file;
        delete frag_s1_file->raw_data;
        delete frag_s1_file;
        loaded_ch1 = true;
    }
    

    SQUE_RENDER_UseProgram(triangle_program1.id);
    //SQUE_RENDER_BindMeshBuffer(triangle1.vert_id, triangle1.index_id, triangle1.attribute_object);
    SQUE_RENDER_DrawVertices(triangle1);
}

float vertices1_2[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices1[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

SQUE_Mesh quad1;
static bool loaded_ch1_2 = false;
void LearnOpenGL_1_2_Indices()
{
    if (!loaded_ch1_2)
    {
        SQUE_MESHES_SetDrawConfig(quad1, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESHES_SetVertData(quad1, 4); // We have 4 vertex for a quad
        SQUE_MESHES_SetIndexData(quad1, 6, SQUE_UINT); // When using indices it is required to initialize amount of indices and index variable type
        SQUE_RENDER_GenerateMeshBuffer(&quad1.vert_id, &quad1.index_id, &quad1.attribute_object);
        SQUE_RENDER_BindMeshBuffer(quad1.vert_id, quad1.index_id, quad1.attribute_object);
        SQUE_MESHES_DeclareAttributes(quad1.vert_id, quad1.attrib_ref, 1);
        SQUE_VertAttrib aPos("aPos", SQUE_FLOAT, false, 3);
        SQUE_MESHES_AddAttribute(quad1.attrib_ref, aPos);

        SQUE_MESHES_CalcVertSize(quad1.attrib_ref);
        SQUE_MESHES_SetLocations(quad1.attrib_ref);
        SQUE_RENDER_SendMeshToGPU(quad1, vertices1_2, indices1);

        SQUE_CHECK_RENDER_ERRORS();
        loaded_ch1_2 = true;
    }
    

    SQUE_RENDER_UseProgram(triangle_program1.id);
    //SQUE_RENDER_BindMeshBuffer(quad1.vert_id, quad1.index_id, quad1.attribute_object);
    SQUE_RENDER_DrawIndices(quad1); // Drawing with indices instead of Vertices
}


// LearnOpenGL_2_Shaders
static bool loaded_ch2 = false;
static bool render_ch2 = true;
SQUE_Mesh triangle2;
SQUE_Program triangle_program2;
SQUE_Shader vert_s2;
SQUE_Shader frag_s2;
SQUE_Asset* vert_s2_file;
SQUE_Asset* frag_s2_file;
void LearnOpenGL_2_Shaders()
{
    if (!loaded_ch2)
    {
        SQUE_MESHES_SetDrawConfig(triangle2, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESHES_SetVertData(triangle2, 3);
        SQUE_RENDER_GenerateMeshBuffer(&triangle2.vert_id, &triangle2.index_id, &triangle2.attribute_object);
        SQUE_RENDER_BindMeshBuffer(triangle2.vert_id, triangle2.index_id, triangle2.attribute_object);
        SQUE_MESHES_DeclareAttributes(triangle2.vert_id, triangle2.attrib_ref, 1);
        SQUE_VertAttrib aPos("aPos", SQUE_FLOAT, false, 3);
        SQUE_MESHES_AddAttribute(triangle2.attrib_ref, aPos);

        SQUE_MESHES_CalcVertSize(triangle2.attrib_ref);
        SQUE_MESHES_SetLocations(triangle2.attrib_ref);
        SQUE_RENDER_SendMeshToGPU(triangle2, vertices1);

        vert_s2_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/vert_s2.vert");
        frag_s2_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/frag_s2.frag");
        std::string vert_source(concat_len(glsl_ver, strlen(glsl_ver), vert_s2_file->raw_data, vert_s2_file->size));
        std::string frag_source(concat_len(glsl_ver, strlen(glsl_ver), frag_s2_file->raw_data, frag_s2_file->size));

        SQUE_SHADERS_Generate(vert_s2, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_s2.id, vert_source.c_str());
        SQUE_SHADERS_Compile(vert_s2.id);

        SQUE_SHADERS_Generate(frag_s2, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_s2.id, frag_source.c_str());
        SQUE_SHADERS_Compile(frag_s2.id);

        // TODO: Have to Change names so that mesh, is for mesh, program for program,...
        // Consistency so that things make sense and are easy to find!
        SQUE_RENDER_CreateProgram(&triangle_program2.id);
        // Type in attaching could be avoided by passing the whole shader directly, it only holds type and id and needs both
        SQUE_PROGRAM_AttachShader(triangle_program2, vert_s2.id, vert_s2.type);
        SQUE_PROGRAM_AttachShader(triangle_program2, frag_s2.id, frag_s2.type);
        SQUE_RENDER_LinkProgram(triangle_program2.id);

        SQUE_SHADERS_FreeFromGPU(frag_s2.id);

        delete frag_s2_file->raw_data;
        delete frag_s2_file;

        loaded_ch2 = true;
    }
    
    if (render_ch2)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_RENDER_UseProgram(triangle_program2.id);
        SQUE_RENDER_DrawVertices(triangle2);
    }
}

static bool loaded_ch2_2 = false;
static bool render_ch2_2 = true;
SQUE_Asset* vert_s2_2_file;
SQUE_Asset* frag_s2_2_file;
SQUE_Timer timer1;
SQUE_Program uniform_program1;
SQUE_Shader frag_uniform_s1;

int32_t ourColor_id;
void LearnOpenGL_2_2_Uniforms()
{
    if (!loaded_ch2_2)
    {
        frag_s2_2_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/frag_s2_2.frag");
        std::string frag_source(concat_len(glsl_ver, strlen(glsl_ver), frag_s2_2_file->raw_data, frag_s2_2_file->size));

        SQUE_SHADERS_Generate(frag_uniform_s1, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_uniform_s1.id, frag_source.c_str());
        SQUE_SHADERS_Compile(frag_uniform_s1.id);


        SQUE_RENDER_CreateProgram(&uniform_program1.id);
        SQUE_PROGRAM_AttachShader(uniform_program1, vert_s2.id, vert_s2.type);
        SQUE_PROGRAM_AttachShader(uniform_program1, frag_uniform_s1.id, frag_uniform_s1.type);
        SQUE_RENDER_LinkProgram(uniform_program1.id);

        SQUE_SHADERS_DeclareProgram(uniform_program1.uniform_ref, uniform_program1.id, 1);
        ourColor_id = SQUE_SHADERS_DeclareUniform(uniform_program1.uniform_ref, uniform_program1.id, "ourColor");
        // TODO: There should be a function to get the id based on the uniform name, non opengl, but to search own struct...


        SQUE_SHADERS_FreeFromGPU(vert_s2.id);
        delete vert_s2_file->raw_data;
        delete vert_s2_file;

        timer1.Start();
        loaded_ch2_2 = true;
    }
    if (render_ch2_2)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        float green = (sin(timer1.ReadMilliSec() / 2.f) + .5f);
        SQUE_RENDER_UseProgram(uniform_program1.id);
        SetFloat(ourColor_id, green);
        SQUE_RENDER_DrawVertices(triangle2);
        SQUE_CHECK_RENDER_ERRORS();
    }
}

static bool loaded_ch2_3 = false;
static bool render_ch2_3 = true;

float vertices2_3[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

SQUE_Mesh triangle2_3;
SQUE_Program attribute_program1;
SQUE_Shader vert_attributes_s1;
SQUE_Shader frag_attributes_s1;
SQUE_Asset* vert_attributes_s1_file;
SQUE_Asset* frag_attributes_s1_file;

void LearnOpenGL_2_3_Attributes()
{
    if (!loaded_ch2_3)
    {
        SQUE_MESHES_SetDrawConfig(triangle2_3, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESHES_SetVertData(triangle2_3, 3);
        SQUE_RENDER_GenerateMeshBuffer(&triangle2_3.vert_id, &triangle2_3.index_id, &triangle2_3.attribute_object);
        SQUE_RENDER_BindMeshBuffer(triangle2_3.vert_id, triangle2_3.index_id, triangle2_3.attribute_object);
        SQUE_MESHES_DeclareAttributes(triangle2_3.vert_id, triangle2_3.attrib_ref, 2); // We setup 2 attributes, Position and Color
        SQUE_VertAttrib aPos("aPos", SQUE_FLOAT, false, 3);
        SQUE_VertAttrib aCol("aColor", SQUE_FLOAT, false, 3);
        SQUE_MESHES_AddAttribute(triangle2_3.attrib_ref, aPos);
        SQUE_MESHES_AddAttribute(triangle2_3.attrib_ref, aCol);

        SQUE_MESHES_CalcVertSize(triangle2_3.attrib_ref);
        SQUE_MESHES_SetLocations(triangle2_3.attrib_ref);
        SQUE_RENDER_SendMeshToGPU(triangle2_3, vertices2_3);

        vert_attributes_s1_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/vert_s2_2.vert");
        frag_attributes_s1_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/frag_s2_3.frag");
        std::string vert_source(concat_len(glsl_ver, strlen(glsl_ver), vert_attributes_s1_file->raw_data, vert_attributes_s1_file->size));
        std::string frag_source(concat_len(glsl_ver, strlen(glsl_ver), frag_attributes_s1_file->raw_data, frag_attributes_s1_file->size));

        SQUE_SHADERS_Generate(vert_attributes_s1, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_Generate(frag_attributes_s1, SQUE_FRAGMENT_SHADER);

        SQUE_SHADERS_SetSource(vert_attributes_s1.id, vert_source.c_str());
        SQUE_SHADERS_SetSource(frag_attributes_s1.id, frag_source.c_str());

        SQUE_SHADERS_Compile(vert_attributes_s1.id);
        SQUE_SHADERS_Compile(frag_attributes_s1.id);

        SQUE_RENDER_CreateProgram(&attribute_program1.id);
        SQUE_PROGRAM_AttachShader(attribute_program1, vert_attributes_s1.id, vert_attributes_s1.type);
        SQUE_PROGRAM_AttachShader(attribute_program1, frag_attributes_s1.id, frag_attributes_s1.type);
        SQUE_RENDER_LinkProgram(attribute_program1.id);

        SQUE_SHADERS_FreeFromGPU(vert_attributes_s1.id);
        SQUE_SHADERS_FreeFromGPU(frag_attributes_s1.id);

        delete vert_attributes_s1_file->raw_data;
        delete vert_attributes_s1_file;
        delete frag_attributes_s1_file->raw_data;
        delete frag_attributes_s1_file;

        loaded_ch2_3 = true;

        SQUE_CHECK_RENDER_ERRORS();
    }
    if (render_ch2_3)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_RENDER_UseProgram(attribute_program1.id);
        SQUE_RENDER_DrawVertices(triangle2_3);
        SQUE_CHECK_RENDER_ERRORS();
    }
}

// LearnOpenGL_3_Textures
static bool loaded_ch3 = false;
static bool render_ch3 = true;

SQUE_Texture2D texture_ch3;
SQUE_Asset* texture_ch3_file;

SQUE_Mesh quad_textured_ch3;
float vertices_ch3[] = {
    // positions          // colors           // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
};

SQUE_Program textured_program_ch3;
SQUE_Asset* vert_texture_s_ch3_file;
SQUE_Asset* frag_texture_ch3_file;
SQUE_Shader vert_texture_s_ch3;
SQUE_Shader frag_texture_ch3;

void LearnOpenGL_3_Textures()
{
    if (!loaded_ch3)
    {

        SQUE_RENDER_GenTextureData(&texture_ch3.id);
        SQUE_RENDER_BindTex2D(texture_ch3.id);
        // TODO: texture format and var type function (SetTexFormat)
        texture_ch3.data_format = texture_ch3.use_format = SQUE_RGB;
        texture_ch3.var_type = SQUE_UBYTE;

        SQUE_TEXTURES_DeclareIntAttributes(texture_ch3.id, 2);
        int nearest = SQUE_NEAREST;
        int linear = SQUE_LINEAR;
        SQUE_TexAttrib min_filter(SQUE_MIN_FILTER, &nearest);
        SQUE_TexAttrib mag_filter(SQUE_MAG_FILTER, &linear);
        SQUE_TEXTURES_AddIntAttribute(texture_ch3.id, min_filter);
        SQUE_TEXTURES_AddIntAttribute(texture_ch3.id, mag_filter);
        SQUE_RENDER_SetTextureAttributes(texture_ch3.id);

        texture_ch3_file = SQUE_FS_LoadAssetRaw("EngineAssets/container.jpeg");
        SQUE_LOAD_Texture(texture_ch3_file, &texture_ch3);
        SQUE_RENDER_SendTex2DToGPU(texture_ch3, texture_ch3_file->raw_data);
        SQUE_FREE_Texture(texture_ch3_file);
        delete texture_ch3_file;

        SQUE_MESHES_SetDrawConfig(quad_textured_ch3, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESHES_SetVertData(quad_textured_ch3, 4);
        SQUE_MESHES_SetIndexData(quad_textured_ch3, 6, SQUE_UINT);
        SQUE_RENDER_GenerateMeshBuffer(&quad_textured_ch3.vert_id, &quad_textured_ch3.index_id, &quad_textured_ch3.attribute_object);
        SQUE_RENDER_BindMeshBuffer(quad_textured_ch3.vert_id, quad_textured_ch3.index_id, quad_textured_ch3.attribute_object);
        SQUE_MESHES_DeclareAttributes(quad_textured_ch3.vert_id, quad_textured_ch3.attrib_ref, 3);
        SQUE_VertAttrib aPos("aPos", SQUE_FLOAT, false, 3);
        SQUE_VertAttrib aCol("aColor", SQUE_FLOAT, false, 3);
        SQUE_VertAttrib aUV("aTexCoord", SQUE_FLOAT, true, 2);
        SQUE_MESHES_AddAttribute(quad_textured_ch3.attrib_ref, aPos);
        SQUE_MESHES_AddAttribute(quad_textured_ch3.attrib_ref, aCol);
        SQUE_MESHES_AddAttribute(quad_textured_ch3.attrib_ref, aUV);

        SQUE_MESHES_CalcVertSize(quad_textured_ch3.attrib_ref);
        SQUE_MESHES_SetLocations(quad_textured_ch3.attrib_ref);
        SQUE_RENDER_SendMeshToGPU(quad_textured_ch3, vertices_ch3, indices1);

        vert_texture_s_ch3_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/vert_s3.vert");
        frag_texture_ch3_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/frag_s3.frag");
        std::string vert_source(concat_len(glsl_ver, strlen(glsl_ver), vert_texture_s_ch3_file->raw_data, vert_texture_s_ch3_file->size));
        std::string frag_source(concat_len(glsl_ver, strlen(glsl_ver), frag_texture_ch3_file->raw_data, frag_texture_ch3_file->size));

        SQUE_SHADERS_Generate(vert_texture_s_ch3, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_texture_s_ch3.id, vert_source.c_str());
        SQUE_SHADERS_Compile(vert_texture_s_ch3.id);

        SQUE_SHADERS_Generate(frag_texture_ch3, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_texture_ch3.id, frag_source.c_str());
        SQUE_SHADERS_Compile(frag_texture_ch3.id);

        SQUE_RENDER_CreateProgram(&textured_program_ch3.id);
        SQUE_PROGRAM_AttachShader(textured_program_ch3, vert_texture_s_ch3.id, vert_texture_s_ch3.type);
        SQUE_PROGRAM_AttachShader(textured_program_ch3, frag_texture_ch3.id, frag_texture_ch3.type);
        SQUE_RENDER_LinkProgram(textured_program_ch3.id);

        delete frag_texture_ch3_file;
        loaded_ch3 = true;
    }

    if (render_ch3)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_RENDER_UseProgram(textured_program_ch3.id);
        
        SQUE_RENDER_SetActiveTextureUnit(SQUE_TEXTURE0);
        SQUE_RENDER_BindTex2D(texture_ch3.id);
        
        SQUE_RENDER_DrawIndices(quad_textured_ch3);
    }
}

static bool loaded_ch3_2 = false;
static bool render_ch3_2 = true;

SQUE_Program textured_program_ch3_2;
SQUE_Shader frag_texture_s_ch3_2;
SQUE_Asset* frag_texture_s_ch3_2_file;
int32_t texture1_uniform;
int32_t texture2_uniform;

SQUE_Texture2D texture_ch3_2;
SQUE_Asset* texture_ch3_2_file;

void LearnOpenGL_3_2_MixTextures()
{
    if (!loaded_ch3_2)
    {
        SQUE_RENDER_GenTextureData(&texture_ch3_2.id);
        SQUE_RENDER_BindTex2D(texture_ch3_2.id);
        texture_ch3_2.data_format = SQUE_RGBA;
        texture_ch3_2.use_format = SQUE_RGB;
        texture_ch3_2.var_type = SQUE_UBYTE;

        SQUE_TEXTURES_DeclareIntAttributes(texture_ch3_2.id, 2);
        int nearest = SQUE_NEAREST;
        int linear = SQUE_LINEAR;
        SQUE_TexAttrib min_filter(SQUE_MIN_FILTER, &nearest);
        SQUE_TexAttrib mag_filter(SQUE_MAG_FILTER, &linear);
        SQUE_TEXTURES_AddIntAttribute(texture_ch3_2.id, min_filter);
        SQUE_TEXTURES_AddIntAttribute(texture_ch3_2.id, mag_filter);
        SQUE_RENDER_SetTextureAttributes(texture_ch3_2.id);

        texture_ch3_2_file = SQUE_FS_LoadAssetRaw("EngineAssets/awesomeface.png");
        SQUE_LOAD_Texture(texture_ch3_2_file, &texture_ch3_2);
        SQUE_RENDER_SendTex2DToGPU(texture_ch3_2, texture_ch3_2_file->raw_data);
        SQUE_FREE_Texture(texture_ch3_2_file);
        delete texture_ch3_2_file;

        frag_texture_s_ch3_2_file = SQUE_FS_LoadAssetRaw("EngineAssets/shaders/frag_s3_2.frag");
        std::string frag_source(concat_len(glsl_ver, strlen(glsl_ver), frag_texture_s_ch3_2_file->raw_data, frag_texture_s_ch3_2_file->size));

        SQUE_SHADERS_Generate(frag_texture_s_ch3_2, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_texture_s_ch3_2.id, frag_source.c_str());
        SQUE_SHADERS_Compile(frag_texture_s_ch3_2.id);

        SQUE_RENDER_CreateProgram(&textured_program_ch3_2.id);
        SQUE_PROGRAM_AttachShader(textured_program_ch3_2, vert_texture_s_ch3.id, vert_texture_s_ch3.type);
        SQUE_PROGRAM_AttachShader(textured_program_ch3_2, frag_texture_s_ch3_2.id, frag_texture_s_ch3_2.type);
        SQUE_RENDER_LinkProgram(textured_program_ch3_2.id);

        SQUE_SHADERS_DeclareProgram(textured_program_ch3_2.uniform_ref, textured_program_ch3_2.id, 2);
        texture1_uniform = SQUE_SHADERS_DeclareUniform(textured_program_ch3_2.uniform_ref, textured_program_ch3_2.id, "texture1");
        texture2_uniform = SQUE_SHADERS_DeclareUniform(textured_program_ch3_2.uniform_ref, textured_program_ch3_2.id, "texture2");

        delete frag_texture_s_ch3_2_file;
        loaded_ch3_2 = true;
    }
    if (render_ch3_2)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_RENDER_UseProgram(textured_program_ch3_2.id);

        SQUE_RENDER_SetActiveTextureUnit(SQUE_TEXTURE0);
        SQUE_RENDER_BindTex2D(texture_ch3.id);

        SQUE_RENDER_SetActiveTextureUnit(SQUE_TEXTURE1);
        SQUE_RENDER_BindTex2D(texture_ch3_2.id);

        SetInt(texture1_uniform, 0);
        SetInt(texture2_uniform, 1);
        // TODO: Have separate bind functions (vertex, indices, attribute_object, and mix of them)

        SQUE_RENDER_DrawIndices(quad_textured_ch3);
    }
}

int main(int argc, char**argv)
{
    main_states state = MAIN_CREATION;
    bool ret = true;

    // SqueLib basic Initialization
    {
        SQUE_LIB_Init("Squelib Testing Grounds");
        SQUE_AddOnGoBackgroundCallback(OnGoBackground);
        SQUE_AddOnResumeCallback(OnResume);

        glsl_ver = SQUE_RENDER_GetGLSLVer();
    }

    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        SQUE_INPUT_Process(0);
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));


        if(on_background)
        {
            SQUE_Sleep(100);
            continue;
        }
        {
            // All LearnOpenGL chapters, each has functions per the steps shown
            // Hello Triangle
            LearnOpenGL_1_Vertices();
            LearnOpenGL_1_2_Indices();
            // Shaders
            LearnOpenGL_2_Shaders();
            LearnOpenGL_2_2_Uniforms();
            LearnOpenGL_2_3_Attributes();
            // Textures
            LearnOpenGL_3_Textures();
            LearnOpenGL_3_2_MixTextures();
        }
        
        SQUE_DISPLAY_SwapAllBuffers();
    }

    // SqueLib CleanUp
    {
        SQUE_LOG(SQUE_LogType::LT_INFO, "Finishing Executing Engine...");
        // Close something that is not part of the engine as a module

        SQUE_LOG(SQUE_LogType::LT_INFO, "Closing Helpers...");
        SQUE_LIB_Close();
    }

    return 0;
}