#include <iostream>
#include <fly_lib.h>
#include <imgui.h>
#include <imgui_impl_flylib.h>

enum main_states
{
	MAIN_CREATION = 0,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

#include <cmath>

int main()
{
    main_states state = MAIN_CREATION;
    bool ret = true;

    // Helpers Initialization
    {
        ret = FLYLIB_Init(/*flags*/);
    }

    // Engine Initialization
    {
        FLYLOG(FLY_LogType::LT_INFO, "Initializing Engine...");
        // Initialize all modules in required order

        state = (ret) ? MAIN_UPDATE : MAIN_EXIT;

        if (ret) { FLYLOG(FLY_LogType::LT_INFO, "Entering Update Loop..."); }
        else FLYLOG(FLY_LogType::LT_WARNING, "Error Initializing Engine...");
    }
    // ImGui Init and Testing
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        ImGui_ImplFlyLib_Init();
    }

    // Follow LeanOpenGL
    {
        
        // Shader Setup
        const char* vertexShaderSource = 
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec3 aCol;\n"
            "out vec3 v_col;\n"
            "void main()\n"
            "{\n"
            "   v_col = aCol;\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";
        FLY_Shader* vert_s = FLYSHADER_Create(FLY_VERTEX_SHADER, 1, &vertexShaderSource, true);
        vert_s->Compile();

        const char* fragmentShaderSource =
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec3 v_col;\n"
            "uniform vec4 ourColor;\n"
            "void main() { FragColor = ourColor+vec4(v_col, 1.0);}\0";
        FLY_Shader* frag_s = FLYSHADER_Create(FLY_FRAGMENT_SHADER, 1, &fragmentShaderSource, true);
        frag_s->Compile();

        FLY_Program program;
        program.Init();
        program.AttachShader(&vert_s);
        program.AttachShader(&frag_s);
        program.Link();
        
        FLY_CHECK_RENDER_ERRORS();
        // Mesh Setup-----------------------------------------------------------------------------------
        float vertices[] = {
             0.5f,  0.5f, 0.0f,  // top right
             0.5f, -0.5f, 0.0f,  // bottom right
            -0.5f, -0.5f, 0.0f,  // bottom left
            -0.5f,  0.5f, 0.0f   // top left 
        };
        unsigned int indices[] = {  // note that we start from 0!
            0, 1, 3,   // first triangle
            1, 2, 3    // second triangle
        };

        FLY_Mesh triangle;
        triangle.verts = (char*)vertices;
        triangle.num_verts = 4;
        triangle.SetDrawMode(FLY_STATIC_DRAW);

        triangle.indices = (char*)indices;
        triangle.num_index = 6;
        triangle.SetIndexVarType(FLY_UINT);

        FLY_VertAttrib* p = triangle.AddAttribute();
        p->SetNumComponents(3);
        p->SetVarType(FLY_FLOAT);
        p->SetNormalize(false);
        p->SetOffset(0);
        /*FLY_VertAttrib* c = new FLY_VertAttrib();
        c->SetNumComponents(3);
        c->SetVarType(FLY_FLOAT);
        c->SetNormalize(false);
        c->SetOffset(p->GetSize());
        */

        triangle.Prepare();
        triangle.SendToGPU();
        triangle.SetLocationsInOrder();

        
        program.Use();
        program.DeclareUniform("ourColor");

        ColorRGBA col = ColorRGBA(0.2f, 0.3f, 0.3f, 1.0f);
        FLY_Timer t;
        bool window = true;
        while (state == MAIN_UPDATE)
        {
            ImGui_ImplFlyLib_NewFrame();
            ImGui::NewFrame();

            FLYRENDER_Clear(NULL, &col);
            program.Use();
            float green = sin(t.ReadMilliSec() / 200.f) + 0.5f;
            program.SetFloat4("ourColor", float4(0,green,0,1));
            program.DrawIndices(&triangle, 0, 6);

            // ImGui Testing
            ImGui::ShowDemoWindow(&window);
            {
                ImGui::Begin("Another Window", &window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    window = false;
                ImGui::End();
            }
            {
                ImGui::Begin("Another Window", &window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    window = false;
                ImGui::End();
            }
            {
                ImGui::Begin("Another Window", &window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    window = false;
                ImGui::End();
            }
            {
                ImGui::Begin("Another Window", &window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
                ImGui::Text("Hello from another window!");
                if (ImGui::Button("Close Me"))
                    window = false;
                ImGui::End();
            }
            ImGui::Render();
            //ImGui::EndFrame();
            FLYDISPLAY_SwapAllBuffers();
            FLYINPUT_Process(0);
            if (FLYDISPLAY_ShouldWindowClose(0))
            {
                FLYLOG(FLY_LogType::LT_INFO, "Checked Window to Close");
                state = MAIN_FINISH;
            }


        }
        triangle.verts = NULL;
        triangle.indices = NULL;
    }

    // For Testing timer and android keyboard
    
    FLYINPUT_DisplaySoftwareKeyboard(true);
    while(state == MAIN_UPDATE)
    {
        

        FLYDISPLAY_SwapAllBuffers();
        

        FLYINPUT_Process(0);
        if (FLYDISPLAY_ShouldWindowClose(0))
        {
            FLYLOG(FLY_LogType::LT_INFO, "Checked Window to Close");
            state = MAIN_FINISH;
        }
        
        
    }

    //  Engine CleanUp
    {
        FLYLOG(FLY_LogType::LT_INFO, "Cleaning Up Engine...");

        // perform the CleanUp of all modules in reverse init order preferably

        if (ret) { FLYLOG(FLY_LogType::LT_INFO, "Application Cleanup Performed Successfully..."); }
        else FLYLOG(FLY_LogType::LT_WARNING, "Application Cleanup with errors...");

        // Delete memory bound modules
    }

    // Helpers CleanUp
    {
        FLYLOG(FLY_LogType::LT_INFO, "Finishing Executing Engine...");
        // Close something that is not part of the engine as a module

        FLYLOG(LT_INFO, "ImGui::Shutdown()");
        ImGui_ImplFlyLib_Shutdown();
        //ImGui::Shutdown(imgui_ctx);

        FLYLOG(FLY_LogType::LT_INFO, "Closing Helpers...");
        FLYLIB_Close();
    }

    return 0;
}