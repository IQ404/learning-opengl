// Standard:

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Externel:

#include <GL/glew.h>        // include this before include gl.h
#include <GLFW/glfw3.h>

// Internel:
#include "Renderer.h"
#include "VBO.h"
#include "IndexBuffer.h"
#include "VAO.h"

struct ShaderProgramSourceCode
{
    std::string VertexShaderSourceCode;
    std::string FragmentShaderSourceCode;
};

static ShaderProgramSourceCode ParseUnifiedShader(const std::string& filepath)
{
    std::ifstream input_stream{ filepath };
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };
    std::string current_line;
    std::stringstream shader_source_code_buffer[2];
    ShaderType current_shader_type = ShaderType::NONE;
    while (std::getline(input_stream, current_line))
    {
        if (current_line.find("#shader") != std::string::npos) // std::string::npos is often implemented as -1
        {
            if (current_line.find("vertex") != std::string::npos)
            {
                current_shader_type = ShaderType::VERTEX;
            }
            else if (current_line.find("fragment") != std::string::npos)
            {
                current_shader_type = ShaderType::FRAGMENT;
            }
            else
            {
                std::cout << "Syntax Error: unspecified shader type in unified shader file." << std::endl;
            }
        }
        else
        {
            shader_source_code_buffer[(int)current_shader_type] << current_line << '\n';
        }
    }
    return { shader_source_code_buffer[0].str(),shader_source_code_buffer[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source_code)
// returns the OpenGL ID of the shader object
{
    GLCall(unsigned int shader_id = glCreateShader(type));
    const char* const src = source_code.c_str();
    GLCall(glShaderSource(shader_id, 1, &src, nullptr));
    GLCall(glCompileShader(shader_id));
    
    /*** Error handling: ***/
    int compile_status;
    GLCall(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status));
    if (compile_status == GL_FALSE)
    {
        int log_length;
        GLCall(glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length));
        char* log = (char*)alloca(log_length * sizeof(char));   // stack-allocated memory
        GLCall(glGetShaderInfoLog(shader_id, log_length, &log_length, log));
        std::cout << "Failed to compile ";
        switch (type)
        {
        case GL_VERTEX_SHADER:
            std::cout << "vertex shader" << std::endl;
            break;
        case GL_FRAGMENT_SHADER:
            std::cout << "fragment shader" << std::endl;
            break;
        default:
            break;
        }
        std::cout << log << std::endl;
        GLCall(glDeleteShader(shader_id));
        return 0;   // A value of 0 for shader will be silently ignored by further OpenGL calls.
    }
    /*** End of error handling ***/

    return shader_id;
}

static unsigned int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
// returns the OpenGL ID of the shader program
{
    GLCall(unsigned int shader_program_id = glCreateProgram());

    unsigned int vertex_shader_id = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fragment_shader_id = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(shader_program_id, vertex_shader_id));
    GLCall(glAttachShader(shader_program_id, fragment_shader_id));

    GLCall(glLinkProgram(shader_program_id));
    GLCall(glValidateProgram(shader_program_id));

    GLCall(glDeleteShader(vertex_shader_id));
    GLCall(glDeleteShader(fragment_shader_id));

    //GLCall(glDetachShader(shader_program_id, vertex_shader_id));
    //GLCall(glDetachShader(shader_program_id, fragment_shader_id));

    return shader_program_id;
}

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

        ShaderProgramSourceCode shader_program_source_code = ParseUnifiedShader("res/shaders/Basic.shader");

        unsigned int shader_program_id = CreateShaderProgram(shader_program_source_code.VertexShaderSourceCode, shader_program_source_code.FragmentShaderSourceCode);
        GLCall(glUseProgram(shader_program_id));

        GLCall(int u_color_id = glGetUniformLocation(shader_program_id, "u_color"));
        //ASSERT_DebugBreak_MSVC(u_color_id != -1);

        //---
        vao.Unbind();
        GLCall(glUseProgram(0));
        index_buffer.Unbind();
        vbo.Unbind();
        //---

        float r = 0.0f;
        float increment = 0.0f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            if (r <= 0.0f)
            {
                increment = 0.05f;
            }
            if (r >= 1.0f)
            {
                increment = -0.05f;
            }
            r += increment;

            //---
            GLCall(glUseProgram(shader_program_id));
            vao.Bind();
            //---

            GLCall(glUniform4f(u_color_id, r, 0.3f, 0.8f, 1.0f));

            // glDrawArrays(GL_TRIANGLES, 0, 6);    // use glDrawArrays when index buffer is not in use
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }

        GLCall(glDeleteProgram(shader_program_id));
    }

    glfwTerminate();
    return 0;
}