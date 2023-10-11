#include "pass_post_processing.h"

using namespace scrinks::render::pass;

PostProcessing::PostProcessing()
    : EffectPass{ "viewport", render::BufferFormat::rgb }
{
}

PostProcessing::~PostProcessing()
{
    RenderPass::~RenderPass();
}

void PostProcessing::load_assets()
{
    m_shader = render::ShaderManager::instance().load_and_store
    (
        "post_process",
        { "assets/shaders/empty_to_quad.vs", "assets/shaders/pp.fs" }
    );
    assert(m_shader);
}

void PostProcessing::setup_draw()
{
    if (!m_shader || m_shader->is_outdated())
        m_shader = render::ShaderManager::instance().get("post_process");

    m_shader->use_program();

    m_shader->set_param("lit", 0);
    m_shader->set_param("depth", 1);

    Buffer::bind(GL_TEXTURE0, "lit");
    Buffer::bind(GL_TEXTURE1, "depth");
}

