#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>

struct ShaderProgramSourceCode
{
    std::string VertexShaderSourceCode;
    std::string FragmentShaderSourceCode;
};

class Shader
{
    std::string m_FilePath;
    unsigned int m_RendererID = 0;
    std::unordered_map<std::string, int> m_UniformLocationCache;

public:

    Shader(const std::string& filepath);

    ~Shader();

    void Bind() const;

    void Unbind() const;

    void SetUniform_4floats(const std::string& u_name, float f1, float f2, float f3, float f4);

private:

    ShaderProgramSourceCode ParseUnifiedShader(const std::string& filepath);

    unsigned int CompileShader(unsigned int type, const std::string& source_code);

    unsigned int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);

    unsigned int GetUniformLocation(const std::string& u_name);
};

#endif // !SHADER_H