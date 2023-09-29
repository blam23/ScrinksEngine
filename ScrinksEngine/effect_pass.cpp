#include "effect_pass.h"

using namespace scrinks::render::pass;

EffectPass::EffectPass(const std::string& bufferName, const render::BufferFormat& format)
    : m_bufferName{ bufferName }
    , m_bufferFormat{ format }
{
}

void EffectPass::init()
{
    load_assets();
}

void EffectPass::draw(float)
{
    glDisable(GL_DEPTH_TEST);

    m_target->bind_write();
    {
        glClearColor(0.0, 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        setup_draw();
        Pipeline::fullscreen_quad();
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void EffectPass::resize(GLsizei width, GLsizei height)
{
    m_target = std::make_unique<render::RenderTarget>(m_bufferName, m_bufferFormat, width, height);
}

