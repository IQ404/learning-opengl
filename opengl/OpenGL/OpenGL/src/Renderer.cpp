#include "Renderer.h"

#include <iostream>
#include "DebugTools.h"

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VAO& vao, const IndexBuffer& index_buffer, const Shader& shader_program) const
{
    shader_program.Bind();
    vao.Bind();
    index_buffer.Bind();

    /*
    Currently:
        - we assume all index buffers contains data of type unsigned int
        - we assume the only primitive we are drawing is triangle
    */
    GLCall(glDrawElements(GL_TRIANGLES, index_buffer.GetIndicesCount(), GL_UNSIGNED_INT, nullptr)); 
}
