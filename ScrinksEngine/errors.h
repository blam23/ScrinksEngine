#pragma once

#include <gl/gl3w.h>
#include <iostream>

namespace scrinks::errors
{
    void glfw_error_callback(int error, const char* description);

    void APIENTRY gl_print_error_callback(
        GLenum source_enum, GLenum type_enum, GLuint id, GLenum severity_enum,
        GLsizei, const GLchar* message, const void* userParam);
}