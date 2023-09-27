#include "pass_geometry.h"
#include "model.h"
#include <glm/ext/matrix_float4x4.hpp>

using namespace scrinks::render::pass;

Geometry::~Geometry()
{
    RenderPass::~RenderPass();
}

void Geometry::init()
{
    RenderPass::init();

    m_shader = render::ShaderManager::instance().load_and_store
    (
        "deferred_render",
        { "assets/shaders/render_single.vs", "assets/shaders/render_single.fs" }
    );

    render::ModelManager::instance().load_and_store("monu2", "D:/Assets/custom/monu2.obj");
}

void Geometry::draw()
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

    // temp hardcoded model
    glm::mat4 model{ glm::mat4(1.0f) };
    m_shader->set_param("model", model);
    m_shader->set_param("view", Pipeline::camera().view());
    m_shader->set_param("projection", Pipeline::projection());

    const auto room{ render::ModelManager::instance().get("monu2") };
    room->draw(m_shader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Geometry::resize(GLsizei, GLsizei)
{
}

