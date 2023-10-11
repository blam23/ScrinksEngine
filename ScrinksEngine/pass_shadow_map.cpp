#include "pass_shadow_map.h"
#include "model.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace scrinks::render::pass;

#include "drawable.h"
#include "node_model.h"

ShadowMap::ShadowMap()
{
}

ShadowMap::~ShadowMap()
{
}

ShadowMap* ShadowMap::init(GLsizei size)
{
    m_size = size;

    m_shader = render::ShaderManager::instance().load_and_store
    (
        "shadow_map",
        { "assets/shaders/shadow_depth.vs", "assets/shaders/empty.fs" }
    );
    assert(m_shader);

	m_target = std::make_unique<render::RenderTarget>("shadow_map", BufferFormat::depth, size, size, GL_DEPTH_ATTACHMENT);

    glBindTexture(GL_TEXTURE_2D, m_target->tex_id());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    return this;
}

#include "debug.h"
glm::mat4 ShadowMap::get_light_space()
{
    float nearPlane{ debug::get_test_float("near plane", -10.0f) };
    float farPlane{ debug::get_test_float("far plane", 10.0f) };
    float size{ debug::get_test_float("size", 10.0f) };
    glm::mat4 projection{ glm::ortho(-size, size, -size, size, nearPlane, farPlane) };

    glm::mat4 view
    { 
        glm::lookAt
        (
            debug::get_test_dir("world light"),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        )
    };

    return projection * view;
}

void ShadowMap::tag_outdated()
{
    m_outdated = true;
}

void ShadowMap::draw(float interpolate)
{
    if (!m_outdated)
        return;

    glEnable(GL_DEPTH_TEST);
    //glCullFace(GL_FRONT);  // peter panning
    glViewport(0, 0, m_size, m_size);

    m_target->bind_write();
    {
        glClear(GL_DEPTH_BUFFER_BIT);

        m_shader->use_program();
        m_shader->set_param<const glm::mat4&>("lightProjection", get_light_space());

        core::nodes::DynamicModel::draw_list(m_shader, interpolate);

        Pipeline::fullscreen_quad();
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glViewport(0, 0, Pipeline::view_width(), Pipeline::view_height());
    
    glCullFace(GL_BACK);

    //m_outdated = false;
}

void ShadowMap::resize(GLsizei, GLsizei)
{
	// no need to resize anything, this is for when the window resizes.
}
