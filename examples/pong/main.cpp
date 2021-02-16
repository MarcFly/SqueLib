#include <squelib.h>
#include <math.h>
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

float quad[] = {
    1,  1,
    1, -1,
    -1, -1,
    -1,  1
};

uint32_t quad_indices[]{
    0,1,3,
    1,2,3
};

struct Paddle {
    float x, y;
    int sizex, sizey;

    SQUE_Mesh rect;
};

struct Ball {
    float x, y;
    int radius;
    float speed;
    float dirx, diry;

    SQUE_Mesh rect;
};

const char* vert_shader = 
    "layout (location = 0) in vec2 vertPos;\n"
    "uniform vec2 center;\n"
    "uniform vec2 size;\n"
    "uniform vec2 vp;\n"
    "out vec2 c;\n"
    "void main()\n"
    "{\n"
        "vec2 ndc_pos = 2.*((center / vp) - vec2(.5));"
        "vec2 ndc_size = ( (vertPos-ndc_pos) / abs(vertPos-ndc_pos) ) * (size / vp);"
        "c = center;"
        "gl_Position = vec4(ndc_pos+ndc_size, 0,1);"
    "}\0";

const char* ball_frag = 
    "out vec4 FragColor;\n"
    "in vec2 c;\n"
    "uniform float radius;\n"
    "uniform vec3 col_in;\n"
    "void main(){\n"
        "float col = step(length(gl_FragCoord.xy-c), radius);\n"
        "FragColor = vec4(col_in,col);\n"
    "}\0";

const char* rect_frag = 
    "out vec4 FragColor;\n"
    "uniform vec3 col;\n"
    "void main(){FragColor = vec4(col,1.);}\0";

void InitParams(Ball& b, Paddle& p1, Paddle& p2)
{
// BALL
    int vx, vy;
    SQUE_DISPLAY_GetViewportSize(0,&vx, &vy);
    b.x = (float)vx / 2.f;
    b.y = (float)vy / 2.f;
    b.radius = 40;
    b.speed = 300;
    // Fault 2 -> Mesh declarations exaggeratedly verbose
    // function that inits: draw_config, draw_mode
    // function that sets: verts + vert_num,  index+index_var+index_num
    // 3 Functions but it makes more sense in the long run (variable verts, variable index, variable draw_config/mode)
    SQUE_MESHES_SetVertData(b.rect, 4);
    SQUE_MESHES_SetIndexData(b.rect, 6, SQUE_UINT);
    SQUE_MESHES_SetDrawConfig(b.rect, SQUE_TRIANGLES, SQUE_STATIC_DRAW);

    
    SQUE_GenerateMeshBuffer(b.rect.vert_id, b.rect.index_id, b.rect.attribute_object);
    SQUE_BindMeshBuffer(b.rect.vert_id, b.rect.index_id, b.rect.attribute_object);
    SQUE_MESHES_DeclareAttributes(b.rect.vert_id, b.rect.attrib_ref, 1);
    SQUE_MESHES_AddAttribute(b.rect.attrib_ref, SQUE_VertAttrib("vertPos", SQUE_FLOAT, false, 2));
    SQUE_MESHES_CalcVertSize(b.rect.attrib_ref);
    SQUE_SendMeshToGPU(b.rect, quad, quad_indices);
    SQUE_MESHES_SetLocations(b.rect.vert_id, b.rect.index_id, b.rect.attribute_object, b.rect.attrib_ref);

// PLAYER 1
    p1.sizex = b.y / 20;
    p1.sizey = b.y / 4;
    p1.x = b.x/20;
    p1.y = vy / 2;
    SQUE_MESHES_SetVertData(p1.rect, 4);
    SQUE_MESHES_SetIndexData(p1.rect, 6, SQUE_UINT);
    SQUE_MESHES_SetDrawConfig(p1.rect, SQUE_TRIANGLES, SQUE_STATIC_DRAW);

    
    SQUE_GenerateMeshBuffer(p1.rect.vert_id, p1.rect.index_id, p1.rect.attribute_object);
    SQUE_BindMeshBuffer(p1.rect.vert_id, p1.rect.index_id, p1.rect.attribute_object);
    SQUE_MESHES_DeclareAttributes(p1.rect.vert_id, p1.rect.attrib_ref, 1);
    SQUE_MESHES_AddAttribute(p1.rect.attrib_ref, SQUE_VertAttrib("vertPos", SQUE_FLOAT, false, 2));
    SQUE_MESHES_CalcVertSize(p1.rect.attrib_ref);
    SQUE_SendMeshToGPU(p1.rect, quad, quad_indices);
    SQUE_MESHES_SetLocations(p1.rect.vert_id, p1.rect.index_id, p1.rect.attribute_object, p1.rect.attrib_ref);

// PLAYER 2
    p2.sizex = p1.sizex;
    p2.sizey = p1.sizey;
    p2.x = vx - b.x/20;
    p2.y = p1.y;
    SQUE_MESHES_SetVertData(p2.rect, 4);
    SQUE_MESHES_SetIndexData(p2.rect, 6, SQUE_UINT);
    SQUE_MESHES_SetDrawConfig(p2.rect, SQUE_TRIANGLES, SQUE_STATIC_DRAW);

    
    SQUE_GenerateMeshBuffer(p2.rect.vert_id, p2.rect.index_id, p2.rect.attribute_object);
    SQUE_BindMeshBuffer(p2.rect.vert_id, p2.rect.index_id, p2.rect.attribute_object);
    SQUE_MESHES_DeclareAttributes(p2.rect.vert_id, p2.rect.attrib_ref, 1);
    SQUE_MESHES_AddAttribute(p2.rect.attrib_ref, SQUE_VertAttrib("vertPos", SQUE_FLOAT, false, 2));
    SQUE_MESHES_CalcVertSize(p2.rect.attrib_ref);
    SQUE_SendMeshToGPU(p2.rect, quad, quad_indices);
    SQUE_MESHES_SetLocations(p2.rect.vert_id, p2.rect.index_id, p2.rect.attribute_object, p2.rect.attrib_ref);
}


char* easy_concat(const char* s1, const char* s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char* result = new char[len1 + len2 + 1]; // +1 for the null-terminator
    // in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
    return result;
}

void InitShaders(SQUE_Program* b_p, SQUE_Program* p_p)
{
    SQUE_RENDER_CreateProgram(b_p);
    SQUE_RENDER_CreateProgram(p_p);

    const char* glsl = SQUE_RENDER_GetGLSLVer();
    std::string vert_source(easy_concat(glsl, vert_shader));
    std::string ball_source(easy_concat(glsl, ball_frag));
    std::string rect_source(easy_concat(glsl, rect_frag));

    SQUE_Shader vert_s2(SQUE_VERTEX_SHADER, vert_source.c_str());
    SQUE_Shader vert_s(SQUE_VERTEX_SHADER, vert_source.c_str());
    SQUE_Shader ball_f(SQUE_FRAGMENT_SHADER, ball_source.c_str());
    SQUE_Shader rect_f(SQUE_FRAGMENT_SHADER, rect_source.c_str());

    SQUE_SHADERS_Compile(vert_s2.id);
    SQUE_SHADERS_Compile(rect_f.id);
    SQUE_PROGRAM_AttachShader(*p_p, vert_s2.id, vert_s2.type);
    SQUE_PROGRAM_AttachShader(*p_p, rect_f.id, rect_f.type);
    SQUE_RENDER_LinkProgram(p_p->id);

    SQUE_SHADERS_Compile(vert_s.id);
    SQUE_SHADERS_Compile(ball_f.id);
    

    SQUE_PROGRAM_AttachShader(*b_p, vert_s.id, vert_s.type);
    SQUE_PROGRAM_AttachShader(*b_p, ball_f.id, ball_f.type);
    SQUE_RENDER_LinkProgram(b_p->id);

    SQUE_CHECK_RENDER_ERRORS();

    
    SQUE_CHECK_RENDER_ERRORS();

    // Fault 1 -> Uniform Declaration should be per shader
    // Then the Program should have a method to set ids for the declared uniforms
    SQUE_RENDER_UseProgram(b_p->id);
    SQUE_SHADERS_DeclareProgram(b_p->id, 5);
    SQUE_SHADERS_DeclareUniform(b_p->id, "vp");
    SQUE_SHADERS_DeclareUniform(b_p->id, "center");
    SQUE_SHADERS_DeclareUniform(b_p->id, "size");
    SQUE_SHADERS_DeclareUniform(b_p->id, "radius");
    SQUE_SHADERS_DeclareUniform(b_p->id, "col_in");

    SQUE_RENDER_UseProgram(p_p->id);
    SQUE_SHADERS_DeclareProgram(p_p->id, 4);
    SQUE_SHADERS_DeclareUniform(p_p->id, "vp");
    SQUE_SHADERS_DeclareUniform(p_p->id, "center");
    SQUE_SHADERS_DeclareUniform(p_p->id, "size");
    SQUE_SHADERS_DeclareUniform(p_p->id, "col");
}

void CleanUpShaders(SQUE_Program& b, SQUE_Program& p)
{
}

void CleanObjects(Ball& b, Paddle& p1, Paddle& p2)
{
}

void BallCollisionWalls(float dt, Ball& b)
{
    int vx, vy;
    SQUE_DISPLAY_GetViewportSize(0, &vx, &vy);
    float bx = b.x + b.dirx*b.speed*dt;
    float by = b.y + b.diry*b.speed*dt;

    if((b.x - b.radius) < 0 || (b.x+b.radius) > vx) 
    {
        b.y = vy/2;
        b.x = vx/2;
        // random ball y dir
    }

    if((b.y - b.radius) < 0 || (b.y + b.radius) > vy)
    {
        b.diry *= -1;
        b.y += b.diry*b.speed*dt;
    }
}

void BallCollisionPaddle(float dt, Paddle& p, Ball& b)
{

    //if (((b.y - b.radius) > p.y && (b.y + b.radius) < (p.y + p.sizey)) && ((b.x - b.radius) > p.x && (b.x + b.radius) < (p.x + p.sizex)))
    // Left is in paddle
    float b_left = b.x - b.radius;
    float b_right = b.x + b.radius;
    float b_top = b.y - b.radius;
    float b_bottom = b.y + b.radius;

    float p_left = p.x - p.sizex;
    float p_right = p.x + p.sizex;
    float p_top = p.y - p.sizey;
    float p_bottom = p.y + p.sizey;
    bool y_in = (b_top < p_bottom && b_top > p_top) || (b_bottom < p_bottom && b_bottom > p_top);
    bool x_in = (b_left < p_right&& b_left > p_left) || (b_right < p_right&& b_right > p_left);
    if( y_in && x_in)
    {
        b.dirx *= -1;
        b.x += b.dirx*b.speed*dt;
    }
}

void ActivePointerCallback(int32_t code, int32_t state)
{
    SQUE_INPUT_SetPointerActive(code, (state > SQUE_ACTION_RELEASE));
}

void MovePaddles(Paddle& p1, Paddle& p2)
{
    float x,y;
    int wx, wy;
    int sx, sy;
    SQUE_DISPLAY_GetWindowPos(0, wx, wy);
    SQUE_DISPLAY_GetWindowSize(0, &sx, &sy);
    if(SQUE_INPUT_GetPointerPos(&x, &y, 0)) 
        p1.y = sy-(y);

    SQUE_INPUT_GetPointerPos(&x, &y, 0);
        p2.y = sy-(y);

}



int main(int argc, char** argv)
{
    SQUE_LIB_Init("SquePong", NULL);
    InitGLDebug();
    SQUE_DISPLAY_SetVSYNC(0);
    SQUE_INPUT_SetMouseButtonCallback(0, ActivePointerCallback);
    SQUE_INPUT_SetMouseButtonCallback(1, ActivePointerCallback);
    Ball ball;
    Paddle player1;
    Paddle player2;
    InitParams(ball, player1, player2);
    
    SQUE_Program ball_program;
    SQUE_Program paddle_program;
    InitShaders(&ball_program, &paddle_program);

    ball.dirx = 1;
    ball.diry = 1;

    double last_time = 0;
    SQUE_Timer timer;
    ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);

    SQUE_RenderState state;
    state.BackUp();
    state.blend = true;
    state.blend_func_dst_alpha = SQUE_ONE_MINUS_SRC_ALPHA;
    state.blend_func_src_alpha = SQUE_SRC_ALPHA;
    state.SetUp();

    while(!SQUE_DISPLAY_ShouldWindowClose(0))
    {
        SQUE_INPUT_Process(0);
        MovePaddles(player1, player2);
        if(on_background)
        {
            SQUE_Sleep(100);
            continue;
        }
        float dt = timer.ReadMilliSec()/1000.;
        timer.Start();

        // Update things
        ball.x += ball.dirx*dt*ball.speed;
        ball.y += ball.diry*dt*ball.speed;
        BallCollisionWalls(dt, ball);
        BallCollisionPaddle(dt, player1, ball);
        BallCollisionPaddle(dt, player2, ball);
        // Render things
        SQUE_RENDER_Clear(col);

        // Ball Render
        SQUE_RENDER_UseProgram(ball_program.id);
        int vx, vy;
        SQUE_DISPLAY_GetViewportSize(0, &vx, &vy);
        SQUE_DISPLAY_MakeContextMain(0);

        float y = vy;
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(ball_program.id, "vp"), glm::vec2(vx,vy));
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(ball_program.id, "center"), glm::vec2(ball.x,ball.y)); // VERT SHADER IS IN CLIP SPACE (-1,1), change and it will be working
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(ball_program.id, "size"), glm::vec2(ball.radius*2, ball.radius*2));
        SetFloat(SQUE_PROGRAM_GetUniformLocation(ball_program.id, "radius"), ball.radius);
        SetFloat3(SQUE_PROGRAM_GetUniformLocation(ball_program.id, "col_in"), glm::vec3(1, 0, 0));

        // state.SetUp();
        // Fault 4 -> SetUp will override ViewPort (duh)
        // If it is not setup as expected when resizing a window or a viewport, setup will then change back the active framebuffer
        // Fault 5 -> SetUp overrides the Program (duh) -> then if you want some quick and dirty it fucks it up
        state.blend = false;
        state.SetUp();
        SQUE_RENDER_DrawIndices(ball.rect);

        SetFloat3(SQUE_PROGRAM_GetUniformLocation(ball_program.id, "col_in"), glm::vec3(0, 1, 1));
        state.blend = true;
        state.SetUp();
        SQUE_RENDER_DrawIndices(ball.rect);
        
        
        // Player 1 Render
        SQUE_RENDER_UseProgram(paddle_program.id);
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "vp"), glm::vec2(vx, vy));
        
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "center"), glm::vec2(player1.x, player1.y));
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "size"), glm::vec2(player1.sizex*2, player1.sizey*2));
        SetFloat3(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "col"), glm::vec3(1, 0, 0));
        SQUE_RENDER_DrawIndices(player1.rect);

        SQUE_RENDER_UseProgram(paddle_program.id);
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "vp"), glm::vec2(vx, vy));

        SetFloat2(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "center"), glm::vec2(player2.x, player2.y));
        SetFloat2(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "size"), glm::vec2(player2.sizex*2, player2.sizey*2));
        SetFloat3(SQUE_PROGRAM_GetUniformLocation(paddle_program.id, "col"), glm::vec3(0, 1, 0));
        SQUE_RENDER_DrawIndices(player2.rect);   

        SQUE_CHECK_RENDER_ERRORS();
        SQUE_DISPLAY_SwapAllBuffers();
    }

    // Fault 3 -> Shader Cleanup
    // When a program is *deleted* it will clean the intern shaders
    // If shaders are shared between programs, one will clean it and others won't know
    // Stuck in loop of deleting memory of shader after ending the execution...

    SQUE_LIB_Close();

    return 0;
}