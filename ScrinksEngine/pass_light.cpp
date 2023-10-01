#include "pass_light.h"
#include "pass_shadow_map.h"

using namespace scrinks::render::pass;

DeferredLighting::DeferredLighting()
    : EffectPass{ "lit", render::BufferFormat::rgb16f }
{
}

DeferredLighting::~DeferredLighting()
{
    RenderPass::~RenderPass();
}

void DeferredLighting::load_assets()
{
    m_shader = render::ShaderManager::instance().load_and_store
    (
        "deferred_lighting_pass",
        { "assets/shaders/empty_to_quad.vs", "assets/shaders/lighting_pass.fs" }
    );
}

void DeferredLighting::setup_draw()
{
    if (!m_shader || m_shader->is_outdated())
        m_shader = render::ShaderManager::instance().get("deferred_lighting_pass");

    m_shader->use_program();

    m_shader->set_param("gPosition", 0);
    m_shader->set_param("gNormal", 1);
    m_shader->set_param("gAlbedoSpec", 2);
    m_shader->set_param("gDepth", 3);
    m_shader->set_param("ssao", 4);
    m_shader->set_param("shadowMap", 5);

    Buffer::bind(GL_TEXTURE0, "position");
    Buffer::bind(GL_TEXTURE1, "normal");
    Buffer::bind(GL_TEXTURE2, "albedo");
    Buffer::bind(GL_TEXTURE3, "depth");
    Buffer::bind(GL_TEXTURE4, "ssao");
    Buffer::bind(GL_TEXTURE5, "shadow_map");

    m_shader->set_param("ambientLightDir", glm::vec3(-2.0, 11.0, 7.0f));
    m_shader->set_param("ambientLightColour", glm::vec3(1.0f, 1.0f, 1.0f));
    m_shader->set_param("ambientLightIntensity", 0.2f);
    m_shader->set_param<const glm::mat4&>("inverseView", glm::inverse(Pipeline::camera().view()));

    if (m_map)
        m_shader->set_param<const glm::mat4&>("lightSpaceMatrix", m_map->get_light_space());

    m_shader->set_test_param("testA");
    m_shader->set_test_param("testB");
    m_shader->set_test_param("testC");
    m_shader->set_test_param("testD");
}

