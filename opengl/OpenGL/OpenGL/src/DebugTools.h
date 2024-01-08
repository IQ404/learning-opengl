#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H

#include <GL/glew.h>        // include this before include gl.h

// Macros:

/*
ASSERT_DebugBreak_MSVC(b): break at the current line if b evaluates to false
    - It is currently MSVC-specific.
    - Add `;` at the end when using it, as its current definition does not end with `;`.
*/
#define ASSERT_DebugBreak_MSVC(b) if (!(b)) __debugbreak()  // __debugbreak is MSVC-specific

/*
GLCall(s): calling OpenGL function with error reporting
    - It is currently MSVC-specific.
    - It will clear all the previously set OpenGL error flags.
    - Add `;` at the end when using it, as its current definition does not end with `;`.
    - Don't write one-line statement using this macro, because its current definition body isn't enclosed with {}.
*/
#define GLCall(s)\
        GLClearErrors();\
        s;\
        ASSERT_DebugBreak_MSVC(!(GLErrorLog(#s, __LINE__, __FILE__)))

void GLClearErrors();

bool GLErrorLog(const char* function_called, int line_calling_from, const char* filepath_calling_from);
// returns true if there is error; false there isn't.

#endif // !DEBUGTOOLS_H
