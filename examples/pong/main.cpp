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
        "vec2 ndc_pos = 2*((center / vp) - vec2(.5));"
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
    b.rect.ChangeVertData(4, (void*)quad);
    b.rect.ChangeIndexData(6, (void*)quad_indices, SQUE_UINT);
    b.rect.ChangeDrawConfig(SQUE_TRIANGLES, SQUE_STATIC_DRAW);
    b.rect.AddAttribute(new SQUE_VertAttrib("vertPos", SQUE_FLOAT, false, 2));
    SQUE_GenerateMeshBuffer(&b.rect);
    SQUE_BindMeshBuffer(b.rect);
    SQUE_SendMeshToGPU(b.rect);
    b.rect.SetLocationsInOrder();

// PLAYER 1
    p1.sizex = b.y / 20;
    p1.sizey = b.y / 4;
    p1.x = b.x/20;
    p1.y = vy / 2;
    p1.rect.ChangeVertData(4, (void*)quad);
    p1.rect.ChangeIndexData(6, (void*)quad_indices);
    p1.rect.ChangeDrawConfig(SQUE_TRIANGLES, SQUE_STATIC_DRAW);
    p1.rect.AddAttribute(new SQUE_VertAttrib("vertPos", SQUE_FLOAT, false, 2));
    SQUE_GenerateMeshBuffer(&p1.rect);
    SQUE_BindMeshBuffer(p1.rect);
    SQUE_SendMeshToGPU(p1.rect);
    p1.rect.SetLocationsInOrder();

// PLAYER 2
    p2.sizex = p1.sizex;
    p2.sizey = p1.sizey;
    p2.x = vx - b.x/20;
    p2.y = p1.y;
    p2.rect.ChangeVertData(4, (void*)quad);
    p2.rect.ChangeIndexData(6, (void*)quad_indices);
    p2.rect.ChangeDrawConfig(SQUE_TRIANGLES, SQUE_STATIC_DRAW);
    p2.rect.AddAttribute(new SQUE_VertAttrib("vertPos", SQUE_FLOAT, false, 2));
    SQUE_GenerateMeshBuffer(&p2.rect);
    SQUE_BindMeshBuffer(p2.rect);
    SQUE_SendMeshToGPU(p2.rect);
    p2.rect.SetLocationsInOrder();
}

void InitShaders(SQUE_Program* b_p, SQUE_Program* p_p)
{
    SQUE_RENDER_CreateProgram(b_p);
    SQUE_RENDER_CreateProgram(p_p);

    const char* glsl = SQUE_RENDER_GetGLSLVer();
    const char* vert_source[2] = {glsl, vert_shader};
    const char* ball_source[2] = {glsl, ball_frag};
    const char* rect_source[2] = {glsl, rect_frag};
    const char* vert_2s[2] = { glsl, vert_shader };

    SQUE_Shader* vert_s2 = new SQUE_Shader(SQUE_VERTEX_SHADER, 2, vert_source);
    SQUE_Shader* vert_s = new SQUE_Shader(SQUE_VERTEX_SHADER, 2, vert_source);
    SQUE_Shader* ball_f = new SQUE_Shader(SQUE_FRAGMENT_SHADER, 2, ball_source);
    SQUE_Shader* rect_f = new SQUE_Shader(SQUE_FRAGMENT_SHADER, 2, rect_source);

    vert_s2->Compile();
    rect_f->Compile();
    p_p->AttachShader(vert_s2);
    p_p->AttachShader(rect_f);
    SQUE_RENDER_LinkProgram(*p_p);

    vert_s->Compile();
    ball_f->Compile();
    

    b_p->AttachShader(vert_s);
    b_p->AttachShader(ball_f);
    SQUE_RENDER_LinkProgram(*b_p);

    SQUE_CHECK_RENDER_ERRORS();

    
    SQUE_CHECK_RENDER_ERRORS();

    // Fault 1 -> Uniform Declaration should be per shader
    // Then the Program should have a method to set ids for the declared uniforms
    SQUE_RENDER_UseProgram(*b_p);
    b_p->DeclareUniform("vp");
    b_p->DeclareUniform("center");
    b_p->DeclareUniform("size");
    b_p->DeclareUniform("radius");
    b_p->DeclareUniform("col_in");

    SQUE_RENDER_UseProgram(*p_p);
    p_p->DeclareUniform("vp");
    p_p->DeclareUniform("center");
    p_p->DeclareUniform("size");
    p_p->DeclareUniform("col");
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

    if((b.x - b.radius)+b.dirx < 0 || (b.x+b.radius) > vx) 
    {
        //b.y = vy/2;
        //b.x = vx/2;
        b.dirx *= -1;
        // random ball y dir
    }

    if((b.y - b.radius) < 0 || (b.y + b.radius) > vy)
    {
        b.diry *= -1;
    }
}

void BallCollisionPaddle(Paddle& p, Ball& b)
{
    
}

int main(int argc, char** argv)
{
    SQUE_LIB_Init("SquePong", NULL);
    SQUE_DISPLAY_SetVSYNC(0);
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
        
        // Render things
        SQUE_RENDER_Clear(col);

        // Ball Render
        SQUE_RENDER_UseProgram(ball_program);
        int vx, vy;
        SQUE_DISPLAY_GetViewportSize(0, &vx, &vy);
        SQUE_DISPLAY_MakeContextMain(0);

        float y = vy;
        SetFloat2(ball_program, "vp", glm::vec2(vx,vy));
        SetFloat2(ball_program, "center", glm::vec2(ball.x,ball.y)); // VERT SHADER IS IN CLIP SPACE (-1,1), change and it will be working
        SetFloat2(ball_program, "size", glm::vec2(ball.radius*2, ball.radius*2));
        SetFloat(ball_program, "radius", ball.radius);
        SetFloat3(ball_program, "col_in", glm::vec3(1, 0, 0));


        // state.SetUp();
        // Fault 4 -> SetUp will override ViewPort (duh)
        // If it is not setup as expected when resizing a window or a viewport, setup will then change back the active framebuffer
        // Fault 5 -> SetUp overrides the Program (duh) -> then if you want some quick and dirty it fucks it up
        state.blend = false;
        //state.SetUp();
        SQUE_RENDER_DrawIndices(ball.rect);

        SetFloat3(ball_program, "col_in", glm::vec3(0, 1, 1));
        state.blend = true;
        //state.SetUp();
        SQUE_RENDER_DrawIndices(ball.rect);
        
        
        // Player 1 Render
        SQUE_RENDER_UseProgram(paddle_program);
        SetFloat2(paddle_program, "vp", glm::vec2(vx, vy));
        
        SetFloat2(paddle_program, "center", glm::vec2(player1.x, player1.y));
        SetFloat2(paddle_program, "size", glm::vec2(player1.sizex, player1.sizey));
        SetFloat3(paddle_program, "col", glm::vec3(1, 0, 0));
        SQUE_RENDER_DrawIndices(player1.rect);

        SQUE_RENDER_UseProgram(paddle_program);
        SetFloat2(paddle_program, "vp", glm::vec2(vx, vy));

        SetFloat2(paddle_program, "center", glm::vec2(player2.x, player2.y));
        SetFloat2(paddle_program, "size", glm::vec2(player2.sizex, player2.sizey));
        SetFloat3(paddle_program, "col", glm::vec3(0, 1, 0));
        SQUE_RENDER_DrawIndices(player2.rect);
        

        SQUE_DISPLAY_SwapAllBuffers();
    }

    // Fault 3 -> Shader Cleanup
    // When a program is *deleted* it will clean the intern shaders
    // If shaders are shared between programs, one will clean it and others won't know
    // Stuck in loop of deleting memory of shader after ending the execution...

    SQUE_LIB_Close();

    return 0;
}