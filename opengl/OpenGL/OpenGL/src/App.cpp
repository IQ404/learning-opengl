// Standard:
#include <iostream>

// Externel:
#include <GL/glew.h>        // include this before include gl.h
#include <GLFW/glfw3.h>

// Internel:
#include "DebugTools.h"
#include "VBO.h"
#include "VBOLayout.h"
#include "IndexBuffer.h"
#include "VAO.h"
#include "Shader.h"
#include "Renderer.h"

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
            -0.5f,-0.5f,    // vertex 1
             0.5f,-0.5f,    // vertex 2
             0.5f, 0.5f,    // vertex 3
            -0.5f, 0.5f     // vertex 4
        };

        unsigned int indices[] =    // OpenGL requires index buffer to store unsigned data!!!
        {
            0,1,2,  // lower-half triangle for the rectangle
            0,2,3   // upper-half triangle for the rectangle
        };

        VAO vao;

        VBO vbo{ vertices, 4 * 2 * sizeof(float) };
        
        VBOLayout vbo_layout;
        vbo_layout.AddAttribute<float>(2);
        
        vao.LinkVertexBuffer(vbo, vbo_layout);

        IndexBuffer index_buffer{ indices, 6 };

        Shader shader{ "res/shaders/Basic.shader" };

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.0f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            if (r <= 0.0f)
            {
                increment = 0.05f;
            }
            if (r >= 1.0f)
            {
                increment = -0.05f;
            }
            r += increment;

            shader.Bind();
            shader.SetUniform_4floats("u_color", r, 0.3f, 0.8f, 1.0f);

            renderer.Draw(vao, index_buffer, shader);

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return 0;
}