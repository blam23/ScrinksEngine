#include "pipeline.h"
#include <glm/ext/matrix_clip_space.hpp>

using namespace scrinks::render;

std::unique_ptr<GBuffer> Pipeline::s_gbuffer{ nullptr };
std::vector<std::unique_ptr<RenderPass>> Pipeline::s_passes{};
GLsizei Pipeline::s_viewWidth{ 0 };
GLsizei Pipeline::s_viewHeight{ 0 };
glm::mat4 Pipeline::s_projection{ 0 };
FPSCamera Pipeline::s_camera{};

GLsizei s_newWidth{ 0 };
GLsizei s_newHeight{ 0 };
bool s_resized{ false };

void Pipeline::draw()
{
    if (s_resized)
        handle_resize();

    if (s_viewWidth == 0 || s_viewHeight == 0)
        return;

    s_projection = glm::perspective(glm::radians(s_camera.fov()), (float)s_viewWidth / (float)s_viewHeight, 0.1f, 100.0f);

    for (auto& pass : s_passes)
        pass->draw();
}

void Pipeline::setup(GLsizei width, GLsizei height)
{
    s_viewWidth = width;
    s_viewHeight = height;
    s_gbuffer = std::make_unique<render::GBuffer>(width, height);
    s_gbuffer->setup_buffers();
}

void Pipeline::handle_resize()
{
    if (s_newWidth == 0 || s_newHeight == 0)
        return;

    setup(s_newWidth, s_newHeight);

    for (auto& pass : s_passes)
        pass->resize(s_newWidth, s_newHeight);

    s_resized = false;
}

void Pipeline::resize(GLsizei width, GLsizei height)
{
    s_newWidth = width;
    s_newHeight = height;
    s_resized = true;
}

void scrinks::render::Pipeline::resize_if_needed(GLsizei width, GLsizei height)
{
    if (s_viewWidth != width || s_viewHeight != height)
        resize(width, height);
}

void scrinks::render::Pipeline::force_recreate()
{
    s_resized = true;
}

GBuffer* Pipeline::gbuffer()
{
    return s_gbuffer.get();
}

void Pipeline::fullscreen_quad()
{
    static GLuint vao{ 0 };

    if (vao == 0)
    {
        //GLuint vbo{ 0 };
        //// pos(3) + uv(2)
        //float quadVertices[] = {
        //    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        //    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        //     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
        //     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        //};
        glGenVertexArrays(1, &vao);
        /*glGenBuffers(1, &vbo);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));*/
    }

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}
