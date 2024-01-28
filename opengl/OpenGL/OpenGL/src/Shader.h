#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct ShaderProgramSourceCode
{
    std::string VertexShaderSourceCode;
    std::string FragmentShaderSourceCode;
};

class Shader
{
    std::string m_FilePath;
    unsigned int m_ShaderProgramID = 0;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;  // a member function marked as const is allowed to modify mutable data members

public:

    Shader(const std::string& filepath);

    ~Shader();

    void Bind() const;

    void Unbind() const;

    void SetUniform_1int(const std::string& u_name, int i1);

    void SetUniform_4floats(const std::string& u_name, float f1, float f2, float f3, float f4);

    void SetUniform_float_matrix_4_4(const std::string& u_name, glm::mat4 matrix);

private:

    ShaderProgramSourceCode ParseUnifiedShader(const std::string& filepath);

    unsigned int CompileShader(unsigned int type, const std::string& source_code);

    unsigned int CreateShaderProgram(const std::string& vertexShader, const std::string& fragmentShader);

    int GetUniformLocation(const std::string& u_name) const;
};

#endif // !SHADER_H