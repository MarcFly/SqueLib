#include <iostream>
#include <squelib.h>
#include <imgui.h>
#include <imgui_impl_squelib.h>
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
static bool render_ch1 = true;
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
        SQUE_MESH_SetDrawConfig(&triangle1, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESH_SetDataConfig(&triangle1, 3);
        SQUE_MESH_GenBuffer(&triangle1);
        SQUE_MESH_BindBuffer(triangle1);
        SQUE_MESH_AddAttribute(&triangle1, "aPos", SQUE_FLOAT, false, 3);
        SQUE_MESH_SetLocations(&triangle1);
        SQUE_MESH_SendToGPU(triangle1, vertices1);

        vert_s1_file = SQUE_FS_LoadAssetRaw("shaders/vert_s1.vert");
        frag_s1_file = SQUE_FS_LoadAssetRaw("shaders/frag_s1.frag");

        SQUE_SHADERS_GenerateID(&vert_s1, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_s1.id, vert_s1_file->raw_data);
        SQUE_SHADERS_Compile(vert_s1.id);

        SQUE_SHADERS_GenerateID(&frag_s1, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_s1.id, frag_s1_file->raw_data);
        SQUE_SHADERS_Compile(frag_s1.id);

        // Have to Change names so that mesh, is for mesh, program for program,...
        // Consistency so that things make sense and are easy to find!
        SQUE_PROGRAM_GenerateID(&triangle_program1.id);
        // Type in attaching could be avoided by passing the whole shader directly, it only holds type and id and needs both
        SQUE_PROGRAM_AttachShader(&triangle_program1, vert_s1);
        SQUE_PROGRAM_AttachShader(&triangle_program1, frag_s1);
        SQUE_PROGRAM_Link(triangle_program1.id);

        SQUE_SHADERS_FreeFromGPU(vert_s1.id);
        SQUE_SHADERS_FreeFromGPU(frag_s1.id);

        delete vert_s1_file->raw_data;
        delete vert_s1_file;
        delete frag_s1_file->raw_data;
        delete frag_s1_file;
        loaded_ch1 = true;
    }
    
    if (render_ch1)
    {
        SQUE_PROGRAM_Use(triangle_program1.id);
        //SQUE_RENDER_BindMeshBuffer(triangle1.vert_id, triangle1.index_id, triangle1.attribute_object);
        SQUE_RENDER_DrawVertices(triangle1);
    }
}

float vertices1_1[] = {
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
static bool loaded_ch1_1 = false;
static bool render_ch1_1 = false;
void LearnOpenGL_1_1_Indices()
{
    if (!loaded_ch1_1)
    {
        SQUE_MESH_SetDrawConfig(&quad1, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESH_SetDataConfig(&quad1, 4, 6, SQUE_UINT); // When using indices it is required to initialize amount of indices and index variable type
        SQUE_MESH_GenBuffer(&quad1);
        //SQUE_MESH_GenBuffer(&quad1.vert_id, &quad1.index_id, &quad1.attribute_object);
        SQUE_MESH_BindBuffer(quad1);
        SQUE_MESH_AddAttribute(&quad1, "aPos", SQUE_FLOAT, false, 3);

        SQUE_MESH_SetLocations(&quad1);
        SQUE_MESH_SendToGPU(quad1, vertices1_1, indices1);

        SQUE_CHECK_RENDER_ERRORS();
        loaded_ch1_1 = true;
    }

    if (render_ch1_1)
    {
        SQUE_PROGRAM_Use(triangle_program1.id);
        //SQUE_RENDER_BindMeshBuffer(quad1.vert_id, quad1.index_id, quad1.attribute_object);
        SQUE_RENDER_DrawIndices(quad1); // Drawing with indices instead of Vertices
    }
}


// LearnOpenGL_2_Shaders
static bool loaded_ch2 = false;
static bool render_ch2 = false;
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
        SQUE_MESH_SetDrawConfig(&triangle2, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESH_SetDataConfig(&triangle2, 3);
        SQUE_MESH_GenBuffer(&triangle2);
        SQUE_MESH_BindBuffer(triangle2);
        SQUE_MESH_AddAttribute(&triangle2, "aPos", SQUE_FLOAT, false, 3);

        SQUE_MESH_SetLocations(&triangle2);
        SQUE_MESH_SendToGPU(triangle2, vertices1);

        vert_s2_file = SQUE_FS_LoadAssetRaw("shaders/vert_s2.vert");
        frag_s2_file = SQUE_FS_LoadAssetRaw("shaders/frag_s2.frag");

        SQUE_SHADERS_GenerateID(&vert_s2, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_s2.id, vert_s2_file->raw_data);
        SQUE_SHADERS_Compile(vert_s2.id);

        SQUE_SHADERS_GenerateID(&frag_s2, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_s2.id, frag_s2_file->raw_data);
        SQUE_SHADERS_Compile(frag_s2.id);

        SQUE_PROGRAM_GenerateID(&triangle_program2.id);
        SQUE_PROGRAM_AttachShader(&triangle_program2, vert_s2);
        SQUE_PROGRAM_AttachShader(&triangle_program2, frag_s2);
        SQUE_PROGRAM_Link(triangle_program2.id);

        SQUE_SHADERS_FreeFromGPU(frag_s2.id);

        delete frag_s2_file->raw_data;
        delete frag_s2_file;

        loaded_ch2 = true;
    }
    
    if (render_ch2)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_PROGRAM_Use(triangle_program2.id);
        SQUE_RENDER_DrawVertices(triangle2);
    }
}

static bool loaded_ch2_1 = false;
static bool render_ch2_1 = false;
SQUE_Asset* vert_s2_1_file;
SQUE_Asset* frag_s2_1_file;
SQUE_Timer timer1;
SQUE_Program uniform_program1;
SQUE_Shader frag_uniform_s1;

int32_t ourColor_id;
void LearnOpenGL_2_1_Uniforms()
{
    if (!loaded_ch2_1)
    {
        frag_s2_1_file = SQUE_FS_LoadAssetRaw("shaders/frag_s2_1.frag");

        SQUE_SHADERS_GenerateID(&frag_uniform_s1, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_uniform_s1.id, frag_s2_1_file->raw_data);
        SQUE_SHADERS_Compile(frag_uniform_s1.id);


        SQUE_PROGRAM_GenerateID(&uniform_program1.id);
        SQUE_PROGRAM_AttachShader(&uniform_program1, vert_s2);
        SQUE_PROGRAM_AttachShader(&uniform_program1, frag_uniform_s1);
        SQUE_PROGRAM_Link(uniform_program1.id);

        SQUE_PROGRAM_CacheUniforms(&uniform_program1);
        ourColor_id = SQUE_PROGRAM_GetUniformID(uniform_program1, "ourColor");

        SQUE_SHADERS_FreeFromGPU(vert_s2.id);
        delete vert_s2_file->raw_data;
        delete vert_s2_file;

        timer1.Start();
        loaded_ch2_1 = true;
    }
    if (render_ch2_1)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        float green = (sin(timer1.ReadMilliSec() / 2.f) + .5f);
        SQUE_PROGRAM_Use(uniform_program1.id);
        SetFloat(ourColor_id, green);
        SQUE_RENDER_DrawVertices(triangle2);
        SQUE_CHECK_RENDER_ERRORS();
    }
}

static bool loaded_ch2_2 = false;
static bool render_ch2_2 = false;

float vertices2_2[] = {
    // positions         // colors
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // top 
};

SQUE_Mesh triangle2_2;
SQUE_Program attribute_program1;
SQUE_Shader vert_attributes_s1;
SQUE_Shader frag_attributes_s1;
SQUE_Asset* vert_attributes_s1_file;
SQUE_Asset* frag_attributes_s1_file;

void LearnOpenGL_2_2_Attributes()
{
    if (!loaded_ch2_2)
    {
        SQUE_MESH_SetDrawConfig(&triangle2_2, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESH_SetDataConfig(&triangle2_2, 3);
        SQUE_MESH_GenBuffer(&triangle2_2);
        //SQUE_RENDER_GenerateMeshBuffer(&triangle2_2.vert_id, &triangle2_2.index_id, &triangle2_2.attribute_object);
        SQUE_MESH_BindBuffer(triangle2_2);
        SQUE_MESH_AddAttribute(&triangle2_2, "aPos", SQUE_FLOAT, false, 3);
        SQUE_MESH_AddAttribute(&triangle2_2, "aColor", SQUE_FLOAT, false, 3);

        SQUE_MESH_SetLocations(&triangle2_2);
        SQUE_MESH_SendToGPU(triangle2_2, vertices2_2);

        vert_attributes_s1_file = SQUE_FS_LoadAssetRaw("shaders/vert_s2_1.vert");
        frag_attributes_s1_file = SQUE_FS_LoadAssetRaw("shaders/frag_s2_2.frag");

        SQUE_SHADERS_GenerateID(&vert_attributes_s1, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_GenerateID(&frag_attributes_s1, SQUE_FRAGMENT_SHADER);

        SQUE_SHADERS_SetSource(vert_attributes_s1.id, vert_attributes_s1_file->raw_data);
        SQUE_SHADERS_SetSource(frag_attributes_s1.id, frag_attributes_s1_file->raw_data);

        SQUE_SHADERS_Compile(vert_attributes_s1.id);
        SQUE_SHADERS_Compile(frag_attributes_s1.id);

        SQUE_PROGRAM_GenerateID(&attribute_program1.id);
        SQUE_PROGRAM_AttachShader(&attribute_program1, vert_attributes_s1);
        SQUE_PROGRAM_AttachShader(&attribute_program1, frag_attributes_s1);
        SQUE_PROGRAM_Link(attribute_program1.id);

        SQUE_SHADERS_FreeFromGPU(vert_attributes_s1.id);
        //SQUE_SHADERS_FreeFromGPU(frag_attributes_s1.id);

        delete vert_attributes_s1_file->raw_data;
        delete vert_attributes_s1_file;
        delete frag_attributes_s1_file->raw_data;
        delete frag_attributes_s1_file;

        loaded_ch2_2 = true;

        SQUE_CHECK_RENDER_ERRORS();
    }
    if (render_ch2_2)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_PROGRAM_Use(attribute_program1.id);
        SQUE_RENDER_DrawVertices(triangle2_2);
        SQUE_CHECK_RENDER_ERRORS();
    }
}

// LearnOpenGL_3_Textures
static bool loaded_ch3 = false;
static bool render_ch3 = false;

SQUE_Texture texture_ch3;
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

        SQUE_TEXTURE_GenBufferIDs(1, &texture_ch3.id);
        SQUE_TEXTURE_SetFormat(&texture_ch3, SQUE_TEXTURE_2D, SQUE_RGB, SQUE_RGB, SQUE_UBYTE);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_AddAttribute(&texture_ch3, "min_filter", SQUE_MIN_FILTER, SQUE_NEAREST);
        SQUE_TEXTURE_AddAttribute(&texture_ch3, "mag_filter", SQUE_MAG_FILTER, SQUE_LINEAR);
        SQUE_TEXTURE_ApplyAttributes(texture_ch3);

        texture_ch3_file = SQUE_FS_LoadAssetRaw("container.jpeg");
        SQUE_LOAD_Texture(texture_ch3_file, &texture_ch3);
        SQUE_TEXTURE_SendAs2DToGPU(texture_ch3, texture_ch3_file->raw_data);
        SQUE_FREE_Texture(texture_ch3_file);
        delete texture_ch3_file;

        SQUE_MESH_SetDrawConfig(&quad_textured_ch3, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESH_SetDataConfig(&quad_textured_ch3, 4, 6, SQUE_UINT);
        SQUE_MESH_GenBuffer(&quad_textured_ch3);
        //SQUE_RENDER_GenerateMeshBuffer(&quad_textured_ch3.vert_id, &quad_textured_ch3.index_id, &quad_textured_ch3.attribute_object);
        SQUE_MESH_BindBuffer(quad_textured_ch3);
        SQUE_MESH_AddAttribute(&quad_textured_ch3, "aPos", SQUE_FLOAT, false, 3);
        SQUE_MESH_AddAttribute(&quad_textured_ch3, "aColor", SQUE_FLOAT, false, 3);
        SQUE_MESH_AddAttribute(&quad_textured_ch3, "aTexCoord", SQUE_FLOAT, true, 2);

        SQUE_MESH_SetLocations(&quad_textured_ch3);
        SQUE_MESH_SendToGPU(quad_textured_ch3, vertices_ch3, indices1);

        vert_texture_s_ch3_file = SQUE_FS_LoadAssetRaw("shaders/vert_s3.vert");
        frag_texture_ch3_file = SQUE_FS_LoadAssetRaw("shaders/frag_s3.frag");

        SQUE_SHADERS_GenerateID(&vert_texture_s_ch3, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_texture_s_ch3.id, vert_texture_s_ch3_file->raw_data);
        SQUE_SHADERS_Compile(vert_texture_s_ch3.id);

        SQUE_SHADERS_GenerateID(&frag_texture_ch3, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_texture_ch3.id, frag_texture_ch3_file->raw_data);
        SQUE_SHADERS_Compile(frag_texture_ch3.id);

        SQUE_PROGRAM_GenerateID(&textured_program_ch3.id);
        SQUE_PROGRAM_AttachShader(&textured_program_ch3, vert_texture_s_ch3);
        SQUE_PROGRAM_AttachShader(&textured_program_ch3, frag_texture_ch3);
        SQUE_PROGRAM_Link(textured_program_ch3.id);

        delete frag_texture_ch3_file;
        loaded_ch3 = true;
    }

    if (render_ch3)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_PROGRAM_Use(textured_program_ch3.id);
        
        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);
        
        SQUE_RENDER_DrawIndices(quad_textured_ch3);
    }
}

static bool loaded_ch3_1 = false;
static bool render_ch3_1 = false;

SQUE_Program textured_program_ch3_1;
SQUE_Shader frag_texture_s_ch3_1;
SQUE_Asset* frag_texture_s_ch3_1_file;
int32_t texture1_uniform;
int32_t texture2_uniform;

SQUE_Texture texture_ch3_1;
SQUE_Asset* texture_ch3_1_file;

void LearnOpenGL_3_1_MixTextures()
{
    if (!loaded_ch3_1)
    {
        SQUE_TEXTURE_GenBufferIDs(1,&texture_ch3_1.id);
        SQUE_TEXTURE_SetFormat(&texture_ch3_1, SQUE_TEXTURE_2D, SQUE_RGBA, SQUE_RGBA, SQUE_UBYTE);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);
        SQUE_TEXTURE_AddAttribute(&texture_ch3_1, "min_filter", SQUE_MIN_FILTER, SQUE_NEAREST);
        SQUE_TEXTURE_AddAttribute(&texture_ch3_1, "mag_filter", SQUE_MAG_FILTER, SQUE_LINEAR);
        SQUE_TEXTURE_ApplyAttributes(texture_ch3_1);
        
        texture_ch3_1_file = SQUE_FS_LoadAssetRaw("awesomeface.png");
        SQUE_LOAD_Texture(texture_ch3_1_file, &texture_ch3_1);
        SQUE_TEXTURE_SendAs2DToGPU(texture_ch3_1, texture_ch3_1_file->raw_data);
        SQUE_FREE_Texture(texture_ch3_1_file);
        delete texture_ch3_1_file;

        frag_texture_s_ch3_1_file = SQUE_FS_LoadAssetRaw("shaders/frag_s3_1.frag");

        SQUE_SHADERS_GenerateID(&frag_texture_s_ch3_1, SQUE_FRAGMENT_SHADER);
        SQUE_SHADERS_SetSource(frag_texture_s_ch3_1.id, frag_texture_s_ch3_1_file->raw_data);
        SQUE_SHADERS_Compile(frag_texture_s_ch3_1.id);

        SQUE_PROGRAM_GenerateID(&textured_program_ch3_1.id);
        SQUE_PROGRAM_AttachShader(&textured_program_ch3_1, vert_texture_s_ch3);
        SQUE_PROGRAM_AttachShader(&textured_program_ch3_1, frag_texture_s_ch3_1);
        SQUE_PROGRAM_Link(textured_program_ch3_1.id);

        SQUE_PROGRAM_CacheUniforms(&textured_program_ch3_1);
        texture1_uniform = SQUE_PROGRAM_GetUniformID(textured_program_ch3_1, "texture1");
        texture2_uniform = SQUE_PROGRAM_GetUniformID(textured_program_ch3_1, "texture2");

        delete frag_texture_s_ch3_1_file;
        loaded_ch3_1 = true;
    }
    if (render_ch3_1)
    {
        SQUE_CHECK_RENDER_ERRORS();
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_PROGRAM_Use(textured_program_ch3_1.id);
SQUE_CHECK_RENDER_ERRORS();
        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);
SQUE_CHECK_RENDER_ERRORS();
        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3.dim_format);
SQUE_CHECK_RENDER_ERRORS();
        SetInt(texture1_uniform, 0);
        SetInt(texture2_uniform, 1);
SQUE_CHECK_RENDER_ERRORS();
        SQUE_RENDER_DrawIndices(quad_textured_ch3);
    }
}


// LearnOpenGL_4_Transformations

static bool loaded_ch4 = false;
static bool render_ch4 = false;

#include<glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

SQUE_Program transform_program_ch4;
SQUE_Shader vert_transform_s_ch4;
SQUE_Asset* vert_transform_s_ch4_file;
glm::mat4 transform_ch4;

int32_t transf_text1_uni;
int32_t transf_text2_uni;
int32_t transf_mat1_uni;
static double last_time;
void LearnOpenGL_4_Transformations()
{
    if (!loaded_ch4)
    {
        transform_ch4 = glm::mat4(1.0f);
        transform_ch4 = glm::rotate(transform_ch4, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
        transform_ch4 = glm::scale(transform_ch4, glm::vec3(0.5, 0.5, 0.5));

        vert_transform_s_ch4_file = SQUE_FS_LoadAssetRaw("shaders/vert_s4.vert");

        SQUE_SHADERS_GenerateID(&vert_transform_s_ch4, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_transform_s_ch4.id, vert_transform_s_ch4_file->raw_data);
        SQUE_SHADERS_Compile(vert_transform_s_ch4.id);

        SQUE_PROGRAM_GenerateID(&transform_program_ch4.id);
        SQUE_PROGRAM_AttachShader(&transform_program_ch4, vert_transform_s_ch4);
        SQUE_PROGRAM_AttachShader(&transform_program_ch4, frag_texture_s_ch3_1);
        SQUE_PROGRAM_Link(transform_program_ch4.id);

        SQUE_PROGRAM_CacheUniforms(&transform_program_ch4);
        transf_text1_uni = SQUE_PROGRAM_GetUniformID(transform_program_ch4, "texture1");
        transf_text2_uni = SQUE_PROGRAM_GetUniformID(transform_program_ch4, "texture2");
        transf_mat1_uni = SQUE_PROGRAM_GetUniformID(transform_program_ch4, "transform");

        SQUE_SHADERS_FreeFromGPU(frag_texture_s_ch3_1.id);

        delete vert_transform_s_ch4_file->raw_data;
        delete vert_transform_s_ch4_file;

        loaded_ch4 = true;
    }
    if (render_ch4)
    {
        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_PROGRAM_Use(transform_program_ch4.id);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);

        SetInt(transf_text1_uni, 0);
        SetInt(transf_text2_uni, 1);

        // Transform over time, not worth to make a full function for it
        double curr_time = timer1.ReadMilliSec();
        transform_ch4 = glm::rotate(transform_ch4, (float)(curr_time - last_time)/100.f, glm::vec3(0.f, 0.f, 1.f));
        last_time = curr_time;
        SetMatrix4(transf_mat1_uni, glm::value_ptr(transform_ch4));

        SQUE_RENDER_DrawIndices(quad_textured_ch3);
    }
}

// LearnOpenGL_5_CoordinateSystems

static bool loaded_ch5 = false;
static bool render_ch5 = false;

glm::mat4 proj_mat_ch5;
glm::mat4 model_mat_ch5;
glm::mat4 view_mat_ch5;

SQUE_Program coordinate_program_ch5;
SQUE_Shader vert_coordinate_s_ch5;
SQUE_Asset* vert_coordinate_s_ch5_file;

int32_t view_uniform_ch5;
int32_t proj_uniform_ch5;
int32_t model_uniform_ch5;
int32_t texture1_uniform_ch5;
int32_t texture2_uniform_ch5;

void LearnOpenGL_5_CoordinateSystems()
{
    if (!loaded_ch5)
    {
        view_mat_ch5 = glm::translate(glm::mat4(1.f) , glm::vec3(0.f, 0.f, -3.f));

        model_mat_ch5 = glm::rotate(glm::mat4(1.f), glm::radians(-55.f), glm::vec3(1.f, 0.f, 0.f));

        vert_coordinate_s_ch5_file = SQUE_FS_LoadAssetRaw("shaders/vert_s5.vert");
        SQUE_SHADERS_GenerateID(&vert_coordinate_s_ch5, SQUE_VERTEX_SHADER);
        SQUE_SHADERS_SetSource(vert_coordinate_s_ch5.id, vert_coordinate_s_ch5_file->raw_data);
        SQUE_SHADERS_Compile(vert_coordinate_s_ch5.id);

        SQUE_PROGRAM_GenerateID(&coordinate_program_ch5.id);
        SQUE_PROGRAM_AttachShader(&coordinate_program_ch5, vert_coordinate_s_ch5);
        SQUE_PROGRAM_AttachShader(&coordinate_program_ch5, frag_texture_s_ch3_1);
        SQUE_PROGRAM_Link(coordinate_program_ch5.id);

        SQUE_PROGRAM_CacheUniforms(&coordinate_program_ch5);
        view_uniform_ch5 = SQUE_PROGRAM_GetUniformID(coordinate_program_ch5, "view");
        proj_uniform_ch5 = SQUE_PROGRAM_GetUniformID(coordinate_program_ch5, "projection");
        model_uniform_ch5 = SQUE_PROGRAM_GetUniformID(coordinate_program_ch5, "model");
        texture1_uniform_ch5 = SQUE_PROGRAM_GetUniformID(coordinate_program_ch5, "texture1");
        texture2_uniform_ch5 = SQUE_PROGRAM_GetUniformID(coordinate_program_ch5, "texture2");
        
        loaded_ch5 = true;

        delete vert_coordinate_s_ch5_file->raw_data;
        delete vert_coordinate_s_ch5_file;
    }
    if (render_ch5)
    {
        int32_t vpx, vpy;
        SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
        proj_mat_ch5 = glm::perspective(glm::radians(45.f), (float)vpx / float(vpy), 0.1f, 100.f);

        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f));
        SQUE_PROGRAM_Use(coordinate_program_ch5.id);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);

        SetInt(texture1_uniform_ch5, 0);
        SetInt(texture1_uniform_ch5, 1);
        
        SetMatrix4(view_uniform_ch5, glm::value_ptr(view_mat_ch5));
        SetMatrix4(model_uniform_ch5, glm::value_ptr(model_mat_ch5));
        SetMatrix4(proj_uniform_ch5, glm::value_ptr(proj_mat_ch5));

        SQUE_RENDER_DrawIndices(quad_textured_ch3);
    }
}

static bool loaded_ch5_1 = false;
static bool render_ch5_1 = false;

float vertices_ch5_1[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
SQUE_Mesh cube_ch5_1;
SQUE_Timer timer_ch5_1;
double last_time_ch5_1;
SQUE_RenderState render_state_ch5_1;
void LearnOpenGL_5_1_3DCubeRotating()
{
    if (!loaded_ch5_1)
    {
        SQUE_MESH_SetDrawConfig(&cube_ch5_1, SQUE_TRIANGLES, SQUE_STATIC_DRAW);
        SQUE_MESH_SetDataConfig(&cube_ch5_1, 36);
        SQUE_MESH_GenBuffer(&cube_ch5_1);
        SQUE_MESH_BindBuffer(cube_ch5_1);
        SQUE_MESH_AddAttribute(&cube_ch5_1, "aPos", SQUE_FLOAT, false, 3);
        SQUE_MESH_AddAttribute(&cube_ch5_1, "aColor", SQUE_FLOAT, false, 0);
        SQUE_MESH_AddAttribute(&cube_ch5_1, "aTexCoord", SQUE_FLOAT, true, 2);
        // For the attributes I wanted to reuse some code, because it was unnecessary at first, but current vertex shaders
        // had MESH with a color attribute. So in this one that is mostly the same but without color attribute
        // I declare it anyways without components, so vertex calculations stay the same but now in shader it will not have data for color
        // so it can't be USED on that mesh!!!

        SQUE_MESH_SetLocations(&cube_ch5_1);
        SQUE_MESH_SendToGPU(cube_ch5_1, vertices_ch5_1);

        render_state_ch5_1.BackUp();
        render_state_ch5_1.depth_test = true;

        loaded_ch5_1 = true;
        timer_ch5_1.Start();
    }
    if (render_ch5_1)
    {
        SQUE_RenderState last;
        last.BackUp();

        render_state_ch5_1.SetUp();

        int32_t vpx, vpy;
        SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
        proj_mat_ch5 = glm::perspective(glm::radians(45.f), (float)vpx / float(vpy), 0.1f, 100.f);

        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
        SQUE_PROGRAM_Use(coordinate_program_ch5.id);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);

        SetInt(texture1_uniform_ch5, 0);
        SetInt(texture1_uniform_ch5, 1);

        SetMatrix4(view_uniform_ch5, glm::value_ptr(view_mat_ch5));
        
        double curr_time = timer1.ReadMilliSec();
        model_mat_ch5 = glm::rotate(model_mat_ch5, ((float)(curr_time - last_time_ch5_1)/1000.f), glm::vec3(.5f, 1.f,0.f));
        last_time_ch5_1 = curr_time;

        SetMatrix4(model_uniform_ch5, glm::value_ptr(model_mat_ch5));
        SetMatrix4(proj_uniform_ch5, glm::value_ptr(proj_mat_ch5));

        SQUE_RENDER_DrawVertices(cube_ch5_1);

        last.SetUp();
    }
}

static bool loaded_ch5_2 = false;
static bool render_ch5_2 = false;

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

double last_time_ch5_2 = 0;

void LearnOpenGL_5_2_More3DCubes()
{
    if (!loaded_ch5_2)
    {
        loaded_ch5_2 = true;
    }
    if (render_ch5_2)
    {
        SQUE_RenderState last;
        last.BackUp();

        render_state_ch5_1.SetUp();

        int32_t vpx, vpy;
        SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
        proj_mat_ch5 = glm::perspective(glm::radians(45.f), (float)vpx / float(vpy), 0.1f, 100.f);

        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
        SQUE_PROGRAM_Use(coordinate_program_ch5.id);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);

        SetInt(texture1_uniform_ch5, 0);
        SetInt(texture1_uniform_ch5, 1);

        SetMatrix4(view_uniform_ch5, glm::value_ptr(view_mat_ch5));
        SetMatrix4(proj_uniform_ch5, glm::value_ptr(proj_mat_ch5));

        for (uint16_t i = 0; i < 10; ++i)
        {
            model_mat_ch5 = glm::translate(glm::mat4(1.f), cubePositions[i]);
            double curr_time = timer1.ReadMilliSec();
            float angle = (i*(curr_time)) / 100.f;
            model_mat_ch5 = glm::rotate(model_mat_ch5, glm::radians(angle), glm::vec3(1.f, .3f, .5f));
            SetMatrix4(model_uniform_ch5, glm::value_ptr(model_mat_ch5));

            SQUE_RENDER_DrawVertices(cube_ch5_1);
        }

        last.SetUp();
    }
}

// LearnOpenGL_6_Camera

static bool loaded_ch6 = false;
static bool render_ch6 = false;

glm::vec3 camera_pos;
glm::vec3 camera_target;
glm::vec3 camera_direction;
glm::vec3 up;
glm::vec3 camera_right;
glm::vec3 camera_up;

glm::mat4 view_mat_ch6;
void LearnOpenGL_6_Camera()
{
    if (!loaded_ch6)
    {
        camera_pos = glm::vec3(0.f, 0.f, 3.f);
        camera_target = glm::vec3(0.f, 0.f, 0.f);
        camera_direction = glm::normalize(camera_pos - camera_target);
        up = glm::vec3(0.f, 1.f, 0.f);
        camera_right = glm::normalize(glm::cross(up, camera_direction));
        camera_up = glm::cross(camera_direction, camera_right);

        //view_mat_ch6 = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,1.f,0.f));

        loaded_ch6 = true;
    }
    if(render_ch6 )
    {
        SQUE_RenderState last;
        last.BackUp();

        render_state_ch5_1.SetUp();

        int32_t vpx, vpy;
        SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
        proj_mat_ch5 = glm::perspective(glm::radians(45.f), (float)vpx / float(vpy), 0.1f, 100.f);

        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
        SQUE_PROGRAM_Use(coordinate_program_ch5.id);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);

        SetInt(texture1_uniform_ch5, 0);
        SetInt(texture1_uniform_ch5, 1);

        
        SetMatrix4(proj_uniform_ch5, glm::value_ptr(proj_mat_ch5));

        // Changes from previous are to th view matrix
        const float radius = 10.f;
        double curr_time = timer1.ReadMilliSec();
        float camX = sin(curr_time / 1000.f)*radius;
        float camZ = cos(curr_time / 1000.f)*radius;
        view_mat_ch6 = glm::lookAt(glm::vec3(camX, 0., camZ), glm::vec3(0., 0., 0.), glm::vec3(0., 1., 0.));
        SetMatrix4(view_uniform_ch5, glm::value_ptr(view_mat_ch6));

        for (uint16_t i = 0; i < 10; ++i)
        {
            model_mat_ch5 = glm::translate(glm::mat4(1.f), cubePositions[i]);
            double curr_time = timer1.ReadMilliSec();
            float angle = (i * (curr_time)) / 100.f;
            model_mat_ch5 = glm::rotate(model_mat_ch5, glm::radians(angle), glm::vec3(1.f, .3f, .5f));
            SetMatrix4(model_uniform_ch5, glm::value_ptr(model_mat_ch5));

            SQUE_RENDER_DrawVertices(cube_ch5_1);
        }

        last.SetUp();

    }
}

static bool loaded_ch6_1 = false;
static bool render_ch6_1 = false;

glm::vec3 camera_pos_ch6_1;
glm::vec3 camera_up_ch6_1;
glm::vec3 camera_front;
double last_time_ch6_1;
float camera_speed;

void LearnOpenGL_6_1_CameraMovement()
{
    if (!loaded_ch6_1)
    {
        camera_pos_ch6_1 = glm::vec3(0.f, 0.f, 3.f);
        camera_front = glm::vec3(0.f, 0.f, -1.f);
        camera_up_ch6_1 = glm::vec3(0.f, 1.f, 0.f);
        loaded_ch6_1 = true;
    }
    // Get Mouse and keyboard input
    double curr_time = timer1.ReadMilliSec();

    {
        camera_speed = 10.f * (curr_time - last_time_ch6_1)/1000.f;
        //SQUE_PRINT(LT_INFO, "%f", (curr_time - last_time_ch6_1) / 1000.f);
        last_time_ch6_1 = curr_time;
        

        SQUE_INPUT_Actions w, a, s, d;
        w = SQUE_INPUT_GetKey(SQUE_KEY_UPPER_W);
        a = SQUE_INPUT_GetKey(SQUE_KEY_UPPER_A);
        s = SQUE_INPUT_GetKey(SQUE_KEY_UPPER_S);
        d = SQUE_INPUT_GetKey(SQUE_KEY_UPPER_D);
        if (w == SQUE_ACTION_PRESS || w == SQUE_ACTION_REPEAT)
            camera_pos_ch6_1 += camera_speed * camera_front;
        if (a == SQUE_ACTION_PRESS || a == SQUE_ACTION_REPEAT)
            camera_pos_ch6_1 -= glm::normalize(glm::cross(camera_front, camera_up_ch6_1)) * camera_speed;
        if (s == SQUE_ACTION_PRESS || s == SQUE_ACTION_REPEAT)
            camera_pos_ch6_1 -= camera_speed * camera_front;
        if (d == SQUE_ACTION_PRESS || d == SQUE_ACTION_REPEAT)
            camera_pos_ch6_1 += glm::normalize(glm::cross(camera_front, camera_up_ch6_1)) * camera_speed;
    }

    if (render_ch6_1)
    {
        SQUE_RenderState last;
        last.BackUp();

        render_state_ch5_1.SetUp();

        int32_t vpx, vpy;
        SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
        proj_mat_ch5 = glm::perspective(glm::radians(45.f), (float)vpx / float(vpy), 0.1f, 100.f);

        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
        SQUE_PROGRAM_Use(coordinate_program_ch5.id);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);

        SetInt(texture1_uniform_ch5, 0);
        SetInt(texture1_uniform_ch5, 1);


        SetMatrix4(proj_uniform_ch5, glm::value_ptr(proj_mat_ch5));

        // Changes from previous are to th view matrix
        

        view_mat_ch6 = glm::lookAt(camera_pos_ch6_1, camera_pos_ch6_1 + camera_front, camera_up_ch6_1);
        SetMatrix4(view_uniform_ch5, glm::value_ptr(view_mat_ch6));

        for (uint16_t i = 0; i < 10; ++i)
        {
            model_mat_ch5 = glm::translate(glm::mat4(1.f), cubePositions[i]);
            float angle = (i * (curr_time)) / 100.f;
            model_mat_ch5 = glm::rotate(model_mat_ch5, glm::radians(angle), glm::vec3(1.f, .3f, .5f));
            SetMatrix4(model_uniform_ch5, glm::value_ptr(model_mat_ch5));

            SQUE_RENDER_DrawVertices(cube_ch5_1);
        }

        last.SetUp();
    }
}

static bool loaded_ch6_2 = false;
static bool render_ch6_2 = false;
glm::vec3 direction_ch6_2;
float yaw, pitch;
double last_time_ch6_2;
float lx, ly;
void LearnOpenGL_6_2_CameraLook()
{
    if (!loaded_ch6_2)
    {
        last_time_ch6_2 = 0;
        yaw = -90.f;
        pitch = 0.f;
        lx = ly = -1;

        loaded_ch6_2 = true;
    }
    double curr_time = timer1.ReadMilliSec();
    if(render_ch6_2){
        float mx = -1, my = -1;
        SQUE_INPUT_GetPointerAvgPos(&mx, &my,10);
        SQUE_INPUT_Actions action = SQUE_INPUT_GetMouseButton(SQUE_MOUSE_LEFT);

        if ((action == SQUE_ACTION_PRESS || action == SQUE_ACTION_REPEAT) && !ImGui::IsAnyItemHovered()) // !ImGui::IsAnyWindowHovered() &&
        {
            yaw += (mx - ((lx == -1) ? mx : lx)) * .1f;
            pitch += (((ly == -1)?my : ly) - my) * .1f;
            
        }

        lx = mx;
        ly = my;


        direction_ch6_2.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction_ch6_2.y = sin(glm::radians(pitch));
        direction_ch6_2.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camera_front = glm::normalize(direction_ch6_2);
    }
    if (render_ch6_2)
    {
        SQUE_RenderState last;
        last.BackUp();

        render_state_ch5_1.SetUp();

        int32_t vpx, vpy;
        SQUE_DISPLAY_GetWindowSize(&vpx, &vpy);
        proj_mat_ch5 = glm::perspective(glm::radians(45.f), (float)vpx / float(vpy), 0.1f, 100.f);

        SQUE_RENDER_Clear(ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f), SQUE_COLOR_BIT | SQUE_DEPTH_BIT);
        SQUE_PROGRAM_Use(coordinate_program_ch5.id);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE0);
        SQUE_TEXTURE_Bind(texture_ch3.id, texture_ch3.dim_format);

        SQUE_TEXTURE_SetActiveUnit(SQUE_TEXTURE1);
        SQUE_TEXTURE_Bind(texture_ch3_1.id, texture_ch3_1.dim_format);

        SetInt(texture1_uniform_ch5, 0);
        SetInt(texture1_uniform_ch5, 1);


        SetMatrix4(proj_uniform_ch5, glm::value_ptr(proj_mat_ch5));

        // Changes from previous are to th view matrix


        view_mat_ch6 = glm::lookAt(camera_pos_ch6_1, camera_pos_ch6_1 + camera_front, camera_up_ch6_1);
        SetMatrix4(view_uniform_ch5, glm::value_ptr(view_mat_ch6));

        for (uint16_t i = 0; i < 10; ++i)
        {
            model_mat_ch5 = glm::translate(glm::mat4(1.f), cubePositions[i]);
            float angle = (i * (curr_time)) / 100.f;
            model_mat_ch5 = glm::rotate(model_mat_ch5, glm::radians(angle), glm::vec3(1.f, .3f, .5f));
            SetMatrix4(model_uniform_ch5, glm::value_ptr(model_mat_ch5));

            SQUE_RENDER_DrawVertices(cube_ch5_1);
        }

        last.SetUp();
    }
}

int main(int argc, char**argv)
{
    bool ret = true;

    // SqueLib basic Initialization
    {
        SQUE_LIB_Init("Squelib Testing Grounds", SQ_INIT_DEFAULTS | SQ_INIT_MAX_RENDER_VER);
        SQUE_AddOnGoBackgroundCallback(OnGoBackground);
        SQUE_AddOnResumeCallback(OnResume);

        InitGLDebug();

        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplSqueLib_Init();
        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale*=SQUE_DISPLAY_GetDPIDensity()/144;
        ImGui::GetStyle().ScaleAllSizes(SQUE_DISPLAY_GetDPIDensity()/144);
    }
    const char* items[] = {
        "1-Vertices", "1.1-Indices", "2-Shaders", "2.1-Uniforms", "2.2-Attributes",
        "3-Textures", "3.1-Mix Textures", "4-Transformations", "5-Coordinate Systems",
        "5.1-3D Cube Rotating", "5.2-More 3D Cubes", "6-Camera", "6.1-Camera Movement",
        "6.2-Camera Look"
    };
    bool* renders[] = { 
        &render_ch1, &render_ch1_1,& render_ch2,& render_ch2_1,& render_ch2_2,
        & render_ch3,& render_ch3_1,& render_ch4,& render_ch5,& render_ch5_1,
        & render_ch5_2,& render_ch6,& render_ch6_1,& render_ch6_2
    };
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
            ImGui_ImplSqueLib_NewFrame();
            ImGui::NewFrame();
            ImGui::Begin("Settings");
            {
                static int current_it = 0;
                if (ImGui::BeginCombo("Example Selection", items[current_it]))
                {
                    for (uint16_t i = 0; i < IM_ARRAYSIZE(items); ++i)
                        if (ImGui::Selectable(items[i], renders[i]))
                        {
                            for (uint16_t j = 0; j < IM_ARRAYSIZE(items); ++j)
                            {
                                *renders[j] = false;
                            }
                            *renders[i] = true;
                            current_it = i;
                        }
                    ImGui::EndCombo();
                }
            }
            ImGui::End();
            ImGui::Render();

            // All LearnOpenGL chapters, each has functions per the steps shown
            // Hello Triangle
            LearnOpenGL_1_Vertices();
            LearnOpenGL_1_1_Indices();
            // Shaders
            LearnOpenGL_2_Shaders();
            LearnOpenGL_2_1_Uniforms();
            LearnOpenGL_2_2_Attributes();
            // Textures
            LearnOpenGL_3_Textures();
            SQUE_CHECK_RENDER_ERRORS();
            LearnOpenGL_3_1_MixTextures();
            SQUE_CHECK_RENDER_ERRORS();
            // Transformations
            LearnOpenGL_4_Transformations();
            // Coordinate Systems and Projection
            LearnOpenGL_5_CoordinateSystems();
            LearnOpenGL_5_1_3DCubeRotating();
            LearnOpenGL_5_2_More3DCubes();
            // Camera
            LearnOpenGL_6_Camera();
            LearnOpenGL_6_1_CameraMovement();
            LearnOpenGL_6_2_CameraLook();

            ImGui_ImplSqueLib_Render(ImGui::GetDrawData());
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