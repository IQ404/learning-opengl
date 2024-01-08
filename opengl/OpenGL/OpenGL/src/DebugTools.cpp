#include "DebugTools.h"

#include <iostream>

void GLClearErrors()
{
    while (glGetError() != GL_NO_ERROR);    // GL_NO_ERROR is guaranteed to be 0
}

bool GLErrorLog(const char* function_called, int line_calling_from, const char* filepath_calling_from)
// returns true if there is error; false there isn't.
{
    if (GLenum error = glGetError())    // enters if block as long as error != 0
    {
        std::cout << "------[OpenGL Error]------" << '\n'
            << "Error GLenum: " << error << '\n'
            << "By executing: " << function_called << '\n'
            << "At line: " << line_calling_from << '\n'
            << "In file: " << filepath_calling_from << '\n'
            << "--------------------------" << std::endl;

        return true;
    }
    return false;
}