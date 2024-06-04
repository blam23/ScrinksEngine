#pragma once

#include "errors.h"
#include "spdlog/spdlog.h"

void scrinks::errors::glfw_error_callback(int error, const char* description)
{
    spdlog::error("GLFW Error {}: {}", error, description);
}

GLuint lastId{ 0 };

void APIENTRY scrinks::errors::gl_print_error_callback(
    GLenum source_enum, GLenum type_enum, GLuint id, GLenum severity_enum,
    GLsizei, const GLchar* message, const void*)
{
    if (severity_enum == GL_DEBUG_SEVERITY_NOTIFICATION || lastId == id)
        return;

    lastId = id;

    std::string source{ "unknown" };
    std::string type{ "unknown" };
    std::string severity{ "unknown" };

    switch (type_enum)
    {
    case GL_DEBUG_TYPE_ERROR:               type = "error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type = "deprecated behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type = "undefined behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         type = "portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         type = "performance"; break;
    case GL_DEBUG_TYPE_OTHER:               type = "other"; break;
    case GL_DEBUG_TYPE_MARKER:              type = "marker"; break;
    }

    switch (source_enum)
    {
    case GL_DEBUG_SOURCE_API:             source = "api"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source = "window"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: source = "shader compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     source = "third party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     source = "application"; break;
    case GL_DEBUG_SOURCE_OTHER:           source = "other"; break;
    }

    switch (severity_enum)
    {
    case GL_DEBUG_SEVERITY_HIGH:         severity = "high"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       severity = "med"; break;
    case GL_DEBUG_SEVERITY_LOW:          severity = "low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: severity = "note"; break;
    }

    spdlog::error("[{}][{}][{}][{}]: {}", severity, id, type, source, message);
}
