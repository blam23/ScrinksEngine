#include "pass_environment_map.h"
#include "model.h"
#include "texture.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace scrinks::render::pass;

#include "drawable.h"
#include "node_model.h"

EnvironmentMap::EnvironmentMap()
{
}

EnvironmentMap::~EnvironmentMap()
{
}

EnvironmentMap* EnvironmentMap::init(GLsizei size)
{
    m_size = size;

    m_shader = render::ShaderManager::load_and_store
    (
        "cubemap",
        { "assets/shaders/cubemap/render.vs", "assets/shaders/cubemap/render.fs" }
    );
    assert(m_shader);

    m_texture = render::TextureManager::load_and_store
    (
        "env_map",
        { "assets/hdr/spiaggia_di_mondello_4k.hdr", render::TextureType::HDR }
    );
    assert(m_texture);

    GLuint rbo;
    glGenFramebuffers(1, &m_fbo);
    glGenRenderbuffers(1, &rbo);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, size, size);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    glGenTextures(1, &m_cubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMap);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (GLuint i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);

    render::BufferManager::store("cube_map", {}, m_cubeMap);

    return this;
}

void EnvironmentMap::tag_outdated()
{
    m_outdated = true;
}

void EnvironmentMap::draw(float /*interpolate*/)
{
    if (!m_outdated)
        return;

    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
        glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
    };

    m_shader->use_program();
    m_shader->set_param<const glm::mat4&>("projection", captureProjection);

    m_texture->bind(GL_TEXTURE0);
    m_shader->set_param("map", 0);

    glViewport(0, 0, m_size, m_size);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    for (unsigned int i = 0; i < 6; ++i)
    {
        m_shader->set_param<const glm::mat4&>("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_cubeMap, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Pipeline::unit_cube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Pipeline::view_width(), Pipeline::view_height());

    m_outdated = false;
}

void EnvironmentMap::resize(GLsizei, GLsizei)
{
    // no need to resize anything, this is for when the window resizes.
}
