#include "sprite_renderer.h"
#include <glm/ext/matrix_clip_space.hpp>

using namespace scrinks::render;

std::shared_ptr<Shader> load_shader()
{
	return ShaderManager::load_and_store
    (
        "sprite_instanced",
        { "assets/shaders/sprite_instanced.vs", "assets/shaders/sprite_instanced.fs" }
    );
}

SpriteRenderer::SpriteRenderer(std::shared_ptr<Texture> texture, int tilesPerRow, size_t instanceCount)
    : InstanceRenderer<SpriteInstance>{ load_shader(), instanceCount }
    , m_tilesPerRow{ tilesPerRow }
    , m_texture{ texture }
{
}

void SpriteRenderer::init()
{
    static std::vector<float> vertexData
    {
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
    };

    bind_vertex_data(vertexData, 4, GL_TRIANGLE_STRIP);
    bind_instance_data();
}

void SpriteRenderer::setup_shader()
{
    static glm::vec2 scale{ 32.f, 32.f };

    m_texture->bind(GL_TEXTURE0);
    m_shader->set_param("tex", 0);
    m_shader->set_param("tiles", m_tilesPerRow);
    m_shader->set_param("scale", scale);
    m_shader->set_param<const glm::mat4&>("projection", glm::ortho(0.0f, (float)Pipeline::view_width(), (float)Pipeline::view_height(), 0.0f, -1.0f, 1.0f));
}

pass::SpriteRender* pass::SpriteRender::init(std::string_view bufferName)
{
    m_bufferName = bufferName;
    return this;
}

void pass::SpriteRender::draw(float /*interpolate*/)
{
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    m_target->bind_write();
    {
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        for (auto& renderer : m_renderers)
            renderer->render();
    }
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void pass::SpriteRender::add_renderer(std::shared_ptr<SpriteRenderer> renderer)
{
    m_renderers.emplace_back(renderer);
}

void pass::SpriteRender::remove_renderer(std::shared_ptr<SpriteRenderer> renderer)
{
    m_renderers.remove(renderer);
}

void pass::SpriteRender::resize(GLsizei width, GLsizei height)
{
    m_target = std::make_unique<render::RenderTarget>(m_bufferName, BufferFormat::rgba16f, width, height);
}

pass::SpriteRender::SpriteRender()
{
}

::pass::SpriteRender::~SpriteRender()
{
}
