// Standard:
#include <iostream>

// Externel:
#include <GL/glew.h>        // include this before include gl.h
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

// Internel:
#include "DebugTools.h"
#include "VBO.h"
#include "VBOLayout.h"
#include "IndexBuffer.h"
#include "VAO.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

int main(void)
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
    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);

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
        float vertices[] =
        {
        //  position:         texCoords:
            -50.0f,-50.0f,    0.0f,0.0f,    // vertex 1
             50.0f,-50.0f,    1.0f,0.0f,    // vertex 2
             50.0f, 50.0f,    1.0f,1.0f,    // vertex 3
            -50.0f, 50.0f,    0.0f,1.0f     // vertex 4
        };

        unsigned int indices[] =    // OpenGL requires index buffer to store unsigned data!!!
        {
            0,1,2,  // lower-half triangle for the rectangle
            0,2,3   // upper-half triangle for the rectangle
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VAO vao;

        VBO vbo{ vertices, 4 * 4 * sizeof(float) };
        
        VBOLayout vbo_layout;
        // position:
        vbo_layout.AddAttribute<float>(2);
        // texture coordinates:
        vbo_layout.AddAttribute<float>(2);
        
        vao.LinkVertexBuffer(vbo, vbo_layout);

        IndexBuffer index_buffer{ indices, 6 };

        // view and projection matrix is placing outside the render loop because the location of our camera and what it sees stay unchange for all frames.
        glm::mat4 view_matrix = glm::translate(glm::mat4{ 1.0f }, glm::vec3{ -320.0f,-240.0f,0.0f });
        glm::mat4 projection_matrix = glm::ortho(-320.0f, 320.0f, -240.0f, 240.0f, -100.0f, 100.0f); // aspect ratio = 4:3, as we are rendering 640 by 480

        // Experiment with glm projection matrices:
        /*glm::vec4 v{ 0.0f,0.0f,10.0f,1.0f };
        glm::vec4 v2{ 0.0f,0.0f,-2.0f,1.0f };
        glm::mat4 trial_projection_matrix_o = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -10.0f, -5.0f);
        glm::vec4 result = trial_projection_matrix_o * v;
        std::cout << "Ortho:\n";
        std::cout << result.x << ", " << result.y << ", " << result.z << std::endl;
        glm::mat4 trial_projection_matrix_p = glm::perspective(45.0f, (GLfloat)640 / (GLfloat)480, 1.0f, 2.0f);
        glm::vec4 result2 = trial_projection_matrix_p * v2;
        std::cout << "Perspective:\n";
        std::cout << (result2.x / result2.w) << ", " << (result2.y / result2.w) << ", " << (result2.z / result2.w) << std::endl;*/

        Shader shader{ "res/shaders/Basic.shader" };

        Texture texture{ "res/textures/brush.png" };
        texture.Bind(0);
        
        shader.Bind();
        shader.SetUniform_1int("u_Texture", 0); // uniform sampler2D is 1 int corresponding to the texture unit (slot)

        // Careful: if we write texture.Unbind(); here, u_Texture will be linked to the default texture object with ID 0!

        Renderer renderer;

        // ImGui Initialization:
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui::StyleColorsDark();
        const char* glsl_version = "#version 150";  // If give error, try 130 might help.
        ImGui_ImplOpenGL3_Init(glsl_version);

        float r = 0.0f;
        float increment = 0.0f;
        glm::vec3 model_world_coordinates{ 320.0f,240.0f,0.0f };

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */

            if (r <= 0.0f)
            {
                increment = 0.05f;
            }
            if (r >= 1.0f)
            {
                increment = -0.05f;
            }
            r += increment;

            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // model matrix is placing inside the render loop because the location of the object we are rendering can change over frames.
            glm::mat4 model_matrix = glm::translate(glm::mat4{ 1.0f }, model_world_coordinates);

            glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

            shader.Bind();
            shader.SetUniform_4floats("u_Color", r, 0.3f, 0.8f, 1.0f);
            shader.SetUniform_float_matrix_4_4("u_MVP", mvp_matrix);

            renderer.Draw(vao, index_buffer, shader);

            {
                ImGui::Begin("Hello, world!");

                ImGui::SliderFloat3("Model's World Coordinates", &model_world_coordinates.x, 0.0f, 700.0f); // Note: glm::vec3's x,y,z are contiguous in memory // TODO: understand its implementation

                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}