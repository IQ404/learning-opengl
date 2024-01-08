#ifndef RENDERER_H
#define RENDERER_H

#include "VAO.h"
#include "IndexBuffer.h"
#include "Shader.h"

class Renderer
{


public:

    void Clear() const;

    void Draw(const VAO& vao, const IndexBuffer& index_buffer, const Shader& shader_program) const;
};

#endif // !RENDERER_H
