// Standard:
#include <iostream>

// Externel:
#include <GL/glew.h>        // include this before include gl.h
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Internel:
#include "tests/TestMenu.h"
#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"

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
    glfwSwapInterval(1);

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
        const char* glsl_version = "#version 130";  // pointing directly to the read-only section of the memory where the compiler allocates storage for string literals
        ImGui_ImplOpenGL3_Init(glsl_version);

        Test::TestBase* current_test = nullptr;
        Test::TestMenu tests_menu{ current_test };
        current_test = &tests_menu;

        tests_menu.AddTest<Test::TestClearColor>("Background Color");
        tests_menu.AddTest<Test::TestTexture2D>("2D Texture");

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
                
                if ((current_test != (&tests_menu)) && (ImGui::Button("<-")))   // Note that we need to put ImGui::Button within the scope between ImGui::Begin and ImGui::End
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
            std::cout << "Closed during testing, deleting the current test..." << std::endl;
            delete current_test;
        }

    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
}