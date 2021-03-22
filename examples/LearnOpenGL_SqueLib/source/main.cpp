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


void LearnOpenGL_1()
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
        SQUE_MESHES_SetLocations(triangle1.vert_id, triangle1.index_id, triangle1.attribute_object, triangle1.attrib_ref);
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
    }
    loaded_ch1 = true;

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
void LearnOpenGL_1_2()
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
        SQUE_MESHES_SetLocations(quad1.vert_id, quad1.index_id, quad1.attribute_object, quad1.attrib_ref);
        SQUE_RENDER_SendMeshToGPU(quad1, vertices1_2, indices1);

        SQUE_CHECK_RENDER_ERRORS();
    }
    loaded_ch1_2 = true;

    SQUE_RENDER_UseProgram(triangle_program1.id);
    //SQUE_RENDER_BindMeshBuffer(quad1.vert_id, quad1.index_id, quad1.attribute_object);
    SQUE_RENDER_DrawIndices(quad1); // Drawing with indices instead of Vertices
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
            // All LearnOpenGL chapters, each a function
            LearnOpenGL_1();
            LearnOpenGL_1_2();
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