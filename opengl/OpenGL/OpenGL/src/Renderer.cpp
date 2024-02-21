#include "Renderer.h"

#include <iostream>
#include "DebugTools.h"

void Renderer::Clear() const
{
    GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::Draw(const VAO& vao, const IndexBuffer& index_buffer, const Shader& shader_program) const
{
    shader_program.Bind();
    vao.Bind();
    index_buffer.Bind();    // this isn't necessary as long as index_buffer has bound to vao somewhere (and without unbind) before this call

    /*
    Currently:
        - we assume all index buffers contains data of type unsigned int
        - we assume the only primitive we are drawing is triangle
    */
    GLCall(glDrawElements(GL_TRIANGLES, index_buffer.GetIndicesCount(), GL_UNSIGNED_INT, nullptr)); 
}

void Renderer::LinearDraw(const VAO& vao, const Shader& shader_program, const unsigned int& count) const
{
    shader_program.Bind();
    vao.Bind();

    GLCall(glDrawArrays(GL_TRIANGLES, 0, count));
}