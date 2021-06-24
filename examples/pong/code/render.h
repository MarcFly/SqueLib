#ifndef _SQUEPONG_RENDER_
#define _SQUEPONG_RENDER_

#include <squelib.h>
//#include <code/objects.h>

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

ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
SQUE_RenderState state;

SQUE_Program ball_program;
SQUE_Program paddle_program;

void InitShaders()
{
    state.BackUp();
    state.blend = true;
    state.blend_func_dst_alpha = SQUE_ONE_MINUS_SRC_ALPHA;
    state.blend_func_src_alpha = SQUE_SRC_ALPHA;
    state.SetUp();

    SQUE_PROGRAM_GenerateID(&ball_program.id);
    SQUE_PROGRAM_GenerateID(&paddle_program.id);

    SQUE_Shader vert_s2;
    SQUE_SHADERS_GenerateID(&vert_s2, SQUE_VERTEX_SHADER);
    SQUE_SHADERS_SetSource(vert_s2.id, vert_shader);

    SQUE_Shader vert_s;
    SQUE_SHADERS_GenerateID(&vert_s, SQUE_VERTEX_SHADER);
    SQUE_SHADERS_SetSource(vert_s.id, vert_shader);

    SQUE_Shader ball_f;
    SQUE_SHADERS_GenerateID(&ball_f, SQUE_FRAGMENT_SHADER);
    SQUE_SHADERS_SetSource(ball_f.id, ball_frag);

    SQUE_Shader rect_f;
    SQUE_SHADERS_GenerateID(&rect_f, SQUE_FRAGMENT_SHADER);
    SQUE_SHADERS_SetSource(rect_f.id, rect_frag);

    SQUE_SHADERS_Compile(vert_s2.id);
    SQUE_SHADERS_Compile(rect_f.id);
    SQUE_PROGRAM_AttachShader(&paddle_program, vert_s2);
    SQUE_PROGRAM_AttachShader(&paddle_program, rect_f);
    SQUE_PROGRAM_Link(paddle_program.id);

    SQUE_SHADERS_Compile(vert_s.id);
    SQUE_SHADERS_Compile(ball_f.id);
    

    SQUE_PROGRAM_AttachShader(&ball_program, vert_s);
    SQUE_PROGRAM_AttachShader(&ball_program, ball_f);
    SQUE_PROGRAM_Link(ball_program.id);

    SQUE_CHECK_RENDER_ERRORS();

    
    SQUE_CHECK_RENDER_ERRORS();

    // Fault 1 -> Uniform Declaration should be per shader
    // Then the Program should have a method to set ids for the declared uniforms
    SQUE_PROGRAM_Use(ball_program.id);
    SQUE_PROGRAM_CacheUniforms(&ball_program);

    SQUE_PROGRAM_Use(paddle_program.id);
    SQUE_PROGRAM_CacheUniforms(&paddle_program);

}

float red[3] = { 1,0,0 };
float cyan[3] = { 0,1,1 };
float green[3] = { 0,1,0 };

void BallRender(int vx, int vy)
{
    SQUE_PROGRAM_Use(ball_program.id);
    float y = vy;
    glm::vec2 v(vx, vy);
    

    SetFloat2(SQUE_PROGRAM_GetUniformID(ball_program, "vp"), &v.x);
    SetFloat2(SQUE_PROGRAM_GetUniformID(ball_program, "center"), &ball.x); // VERT SHADER IS IN CLIP SPACE (-1,1), change and it will be working
    glm::vec2 bsize(ball.radius * 2, ball.radius * 2);
    SetFloat2(SQUE_PROGRAM_GetUniformID(ball_program, "size"), &bsize.x);// glm::vec2(ball.radius * 2, ball.radius * 2));
    SetFloat(SQUE_PROGRAM_GetUniformID(ball_program, "radius"), ball.radius);
    SetFloat3(SQUE_PROGRAM_GetUniformID(ball_program, "col_in"), red);

    SetFloat3(SQUE_PROGRAM_GetUniformID(ball_program, "col_in"),cyan);
    SQUE_RENDER_DrawIndices(ball.rect);
}

extern Ball ball;
extern Paddle player1;
extern Paddle player2;


void PlayersRender(int vx, int vy)
{
    // Player 1 Render
    glm::vec2 v(vx, vy);

    SQUE_PROGRAM_Use(paddle_program.id);
    SetFloat2(SQUE_PROGRAM_GetUniformID(paddle_program, "vp"), &v.x);
        
    SetFloat2(SQUE_PROGRAM_GetUniformID(paddle_program, "center"), &player1.x);
    glm::vec2 p1size(player1.sizex * 2, player1.sizey * 2);
    SetFloat2(SQUE_PROGRAM_GetUniformID(paddle_program, "size"), &p1size.x);
    //
    SetFloat3(SQUE_PROGRAM_GetUniformID(paddle_program, "col"), red);
    SQUE_RENDER_DrawIndices(player1.rect);

    SQUE_PROGRAM_Use(paddle_program.id);
    SetFloat2(SQUE_PROGRAM_GetUniformID(paddle_program, "vp"), &v.x);

    SetFloat2(SQUE_PROGRAM_GetUniformID(paddle_program, "center"), &player2.x);
    glm::vec2 p2size(player2.sizex * 2, player2.sizey * 2);
    SetFloat2(SQUE_PROGRAM_GetUniformID(paddle_program, "size"), &p2size.x);
    SetFloat3(SQUE_PROGRAM_GetUniformID(paddle_program, "col"), green);
    SQUE_RENDER_DrawIndices(player2.rect);  
}

#endif