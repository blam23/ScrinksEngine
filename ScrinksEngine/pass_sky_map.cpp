#include "pass_sky_map.h"

using namespace scrinks::render::pass;

SkyMap::SkyMap()
{
}

SkyMap::~SkyMap()
{
    RenderPass::~RenderPass();
}

void SkyMap::init()
{
    m_shader = render::ShaderManager::instance().load_and_store
    (
        "sky_map",
        { "assets/shaders/sky_map.vs", "assets/shaders/sky_map.fs" }
    );

    assert(m_shader);
}

void SkyMap::draw(float /*interpolate*/)
{
    if (!m_shader || m_shader->is_outdated())
        m_shader = render::ShaderManager::instance().get("sky_map");

    Pipeline::gbuffer()->bind_write();
    {
        m_shader->use_program();

        m_shader->set_param<const glm::mat4&>("view", Pipeline::camera().view());
        m_shader->set_param<const glm::mat4&>("projection", Pipeline::projection());
        Pipeline::unit_cube();
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

