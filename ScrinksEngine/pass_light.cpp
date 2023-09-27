#include "pass_light.h"

using namespace scrinks::render::pass;

DeferredLighting::~DeferredLighting()
{
    RenderPass::~RenderPass();
}

void DeferredLighting::init()
{
    RenderPass::init();

    m_shader = render::ShaderManager::instance().load_and_store
    (
        "deferred_lighting_pass",
        { "assets/shaders/empty_to_quad.vs", "assets/shaders/lighting_pass.fs" }
    );
}

void DeferredLighting::draw()
{
    glDisable(GL_DEPTH_TEST);

    m_target->bind_write();
    {
        glClearColor(0.0, 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        if (!m_shader || m_shader->is_outdated())
            m_shader = render::ShaderManager::instance().get("deferred_lighting_pass");

        m_shader->use_program();

        m_shader->set_param("gPosition", 0);
        m_shader->set_param("gNormal", 1);
        m_shader->set_param("gAlbedoSpec", 2);
        m_shader->set_param("ssao", 3);

        Buffer::bind(GL_TEXTURE0, "position");
        Buffer::bind(GL_TEXTURE1, "normal");
        Buffer::bind(GL_TEXTURE2, "albedo");
        Buffer::bind(GL_TEXTURE3, "ssao");

        // send light relevant uniforms
        //for (unsigned int i = 0; i < lightPositions.size(); i++)
        //{
        //    lightingShader->set_param("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        //    lightingShader->set_param("lights[" + std::to_string(i) + "].Color", lightColors[i]);
        //    // update attenuation parameters and calculate radius
        //    const float linear = 0.7f;
        //    const float quadratic = 1.8f;
        //    lightingShader->set_param("lights[" + std::to_string(i) + "].Linear", linear);
        //    lightingShader->set_param("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        //}

        m_shader->set_param("ambientLightDir", glm::vec3(0.6f, 0.7f, -0.7f));
        m_shader->set_param("ambientLightColour", glm::vec3(1.0f, 1.0f, 1.0f));
        m_shader->set_param("ambientLightIntensity", 0.2f);
        m_shader->set_param("view", Pipeline::camera().view());
        m_shader->set_test_param("testA");
        m_shader->set_test_param("testB");
        m_shader->set_test_param("testC");
        m_shader->set_test_param("testD");

        Pipeline::fullscreen_quad();
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void DeferredLighting::resize(GLsizei width, GLsizei height)
{
    m_target = std::make_unique<render::RenderTarget>("viewport", render::BufferFormat::rgb, width, height);
}
