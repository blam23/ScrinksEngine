#include "pass_geometry.h"
#include "model.h"

#pragma warning(push)
#pragma warning(disable: 4201)
#include <glm/ext.hpp>
#pragma warning(pop)

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

    render::ModelManager::instance().load_and_store("cube", "D:/Assets/voxel/10_cube_basic.obj");
    render::ModelManager::instance().load_and_store("monu2", "D:/Assets/custom/monu2.obj");
    render::ModelManager::instance().load_and_store("torus", "assets/models/torus.obj");
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

    m_shader->set_param("view", Pipeline::camera().view());
    m_shader->set_param("projection", Pipeline::projection());
    const auto room{ render::ModelManager::instance().get("torus") };
    const auto cube{ render::ModelManager::instance().get("cube") };

    glm::mat4 model{ glm::mat4(1.0f) };

    for (int z = -1; z < 1; z++)
    {
        for (int x = -1; x < 1; x++)
        {
            glm::vec3 pos{ x * 3, 1.0f, z * 3 };
            m_shader->set_param("model", glm::translate(model, pos));
            room->draw(m_shader);
        }
    }

    //glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
    //m_shader->set_param("model", glm::translate(model, pos));
    //room->draw(m_shader);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void Geometry::resize(GLsizei, GLsizei)
{
}

