#include "pass_geometry.h"

#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/ext.hpp>
#pragma warning(pop)

#include "drawable.h"
#include "node_model.h"

using namespace scrinks::render::pass;

Geometry::~Geometry()
{
    RenderPass::~RenderPass();
}

void Geometry::init()
{
    m_shader = render::ShaderManager::instance().load_and_store
    (
        "deferred_render",
        { "assets/shaders/render_single.vs", "assets/shaders/render_single.fs" }
    );
}

void Geometry::draw(float interpolate)
{
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, Pipeline::view_width(), Pipeline::view_height());
    glEnable(GL_CULL_FACE);

    Pipeline::gbuffer()->bind_write();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_shader)
    {
        if (m_shader->is_outdated())
            m_shader = render::ShaderManager::instance().get("deferred_render");

        m_shader->use_program();
    }

    m_shader->set_param("view", Pipeline::camera().view());
    m_shader->set_param("projection", Pipeline::projection());

    render::Drawable<core::nodes::DynamicModel>::draw_list(m_shader, interpolate);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Geometry::resize(GLsizei, GLsizei)
{
}

