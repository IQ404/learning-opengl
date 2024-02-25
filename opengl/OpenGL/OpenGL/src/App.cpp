// Standard:
#include <iostream>

// Externel:
#include <GL/glew.h>        // include this before include gl.h
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Internel:
#include "tests/TestHeaders.h"

int main()
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "Error: glfwInit() failed to initialize" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);  // TODO: why context version not working with compatibility mode?
        
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(640*3, 480*3, "Learning OpenGL", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        std::cout << "Error: glfwCreateWindow() failed to create the window" << std::endl;
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);         // create OpenGL rendering context

    if (glewInit() != GLEW_OK)              // Do glewInit() only after we have a valid OpenGL rendering context
    {
        std::cout << "Error: glewInit() != GLEW_OK" << std::endl;
        //return -1;
        // TODO: should we return -1 here?
    }

    /* V-Sync */
    glfwSwapInterval(0);

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    GLCall(std::cout << "OpenGL version: " << glGetString(GL_VERSION) << '\n' << std::endl);

    /*GLint maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    std::cout << maxVertexAttribs << std::endl;*/

    //---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    {
        Renderer global_renderer;

        // ImGui Initialization:
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        const char* glsl_version = "#version 460 core";  // pointing directly to the read-only section of the memory where the compiler allocates storage for string literals
        ImGui_ImplOpenGL3_Init(glsl_version);

        Test::TestBase* current_test = nullptr;
        Test::TestMenu tests_menu{ current_test };
        current_test = &tests_menu;

        tests_menu.AddTest<Test::TestClearColor>("Background Color");
        tests_menu.AddTest<Test::TestTexture2D>("2D Texture");
        tests_menu.AddTest<Test::TestSmoothstepTexture>("Smoothstep Texture");
        tests_menu.AddTest<Test::TestStepSmoothstepMix>("step, smoothstep, mix");
        tests_menu.AddTest<Test::TestGrid>("Grid");
        tests_menu.AddTest<Test::TestTVScanLines>("TV Scan Lines");
        tests_menu.AddTest<Test::TestAmbient>("Ambient/Hemisphere Lighting");
        tests_menu.AddTest<Test::TestLambertian>("Lambertian Lighting & sRGB Correction");
        tests_menu.AddTest<Test::TestPhong>("A Hacky Phong/Blinn-Phong Shading");
        tests_menu.AddTest<Test::TestSkybox>("Skybox");
        tests_menu.AddTest<Test::TestEnvironmentMapping>("Environment Mapping & Fresnel Effect");
        tests_menu.AddTest<Test::TestCelShading>("Cel Shading");
        tests_menu.AddTest<Test::TestOffsetScalingRotation>("Offset/Scaling/Rotation");
        tests_menu.AddTest<Test::TestEasingAnimation>("Model Animation using Easing Functions");
        tests_menu.AddTest<Test::TestRipplingSphere>("Rippling Sphere");
        tests_menu.AddTest<Test::TestSDF2D>("2D SDF");

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            global_renderer.Clear();

            // ImGui refresh:
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            if (current_test)   // TODO: do we need to test for nullptr here?
            {
                current_test->OnUpdate(0.0f);
                current_test->OnRender();

                ImGui::Begin(current_test->GetTestName().c_str());
                //ImGui::Begin("Test");
                
                if ((current_test != (&tests_menu)) && (ImGui::Button("<-back to tests menu")))   // Note that we need to put ImGui::Button within the scope between ImGui::Begin and ImGui::End
                {
                    delete current_test;
                    current_test = &tests_menu;
                }
                current_test->OnImGuiRender();
                
                ImGui::End();
                
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();   // should we put this at the beginning of the rendering loop?
            // TODO: understand why the ImGui window isn't showed up on the canvas at this point in the first iteration of the render loop.

        }

        if (current_test != (&tests_menu))
        {
            std::cout << "\nClosed during testing, deleting the current test..." << std::endl;
            delete current_test;
        }

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}