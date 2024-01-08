#include "Shader.h"
#include "DebugTools.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& filepath)
	: m_FilePath{ filepath }
{
    ShaderProgramSourceCode shader_program_source_code = ParseUnifiedShader(m_FilePath);

    m_RendererID = CreateShaderProgram(shader_program_source_code.VertexShaderSourceCode, shader_program_source_code.FragmentShaderSourceCode);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(m_RendererID));
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0));
}

unsigned int Shader::GetUniformLocation(const std::string& u_name)
{
    if (m_UniformLocationCache.find(u_name) != m_UniformLocationCache.end())
    // the uniform's id is in our cache:
    {
        return m_UniformLocationCache[u_name];
    }
    // the uniform's id is not in our cache:
    GLCall(int u_id = glGetUniformLocation(m_RendererID, u_name.c_str()));
    //ASSERT_DebugBreak_MSVC(u_color_id != -1);
    if (u_id == -1)
    {
        std::cout << "Warning: uniform '" << u_name << "' doesn't exist in the shader program." << std::endl;
    }
    // cache the uniform's id before returning:
    m_UniformLocationCache[u_name] = u_id;
    // Note: here we assume that the existence of uniform will not change unless the shader is recompiled.
    return u_id;
}

void Shader::SetUniform_4floats(const std::string& u_name, float f1, float f2, float f3, float f4)
{
    GLCall(glUniform4f(GetUniformLocation(u_name), f1, f2, f3, f4));
}

ShaderProgramSourceCode Shader::ParseUnifiedShader(const std::string& filepath)
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

unsigned int Shader::CompileShader(unsigned int type, const std::string& source_code)
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

unsigned int Shader::CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader)
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


