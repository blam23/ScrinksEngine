#include "pass_ssao.h"
#include "debug.h"
#include <glm/geometric.hpp>

using namespace scrinks::render::pass;

std::uniform_real_distribution<float> s_randomFloat{ 0.0, 1.0 };
std::default_random_engine s_generator{};

SSAO::SSAO()
	: EffectPass{ "ssao_raw", render::BufferFormat::red }
{
}

SSAO::~SSAO()
{
	if (m_noiseTexture != 0)
		glDeleteTextures(1, &m_noiseTexture);

	RenderPass::~RenderPass();
}

void SSAO::configure_kernel_count(size_t kernels)
{
	m_kernels_requested = kernels;

	// Re-initialise if required
	if (m_initialised)
		init();
}

float lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

void SSAO::load_assets()
{
	m_kernel.clear();
	m_kernel.reserve(m_kernels_requested);
	for (size_t i = 0; i < m_kernels_requested; i++)
	{
		glm::vec3 sample
		{
			s_randomFloat(s_generator) * 2.0f - 1.0f,
			s_randomFloat(s_generator) * 2.0f - 1.0f,
			s_randomFloat(s_generator),
		};

		sample = glm::normalize(sample);
		sample *= s_randomFloat(s_generator);
		float scale = (float)i / m_kernels_requested;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_kernel.push_back(sample);
	}

	if (!m_initialised)
	{
		generate_noise_texture();

		m_shader = render::ShaderManager::instance().load_and_store(
			"ssao", { "assets/shaders/empty_to_quad.vs", "assets/shaders/ssao.fs" }
		);

		send_data_to_shader();
	}

	RenderPass::init();
}

void SSAO::send_data_to_shader()
{
	size_t i{ 0 };
	for (const auto& sample : m_kernel)
	{
		m_shader->set_param("samples[" + std::to_string(i) + "]", sample);
		i++;
	}
}

void SSAO::setup_draw()
{
	if (!m_shader || m_shader->is_outdated())
		m_shader = render::ShaderManager::instance().get("ssao");

	m_shader->use_program();
	m_shader->set_param("screenWidth", Pipeline::view_width());
	m_shader->set_param("screenHeight", Pipeline::view_height());
	m_shader->set_param("projection", Pipeline::projection());
	m_shader->set_param("view", Pipeline::camera().view());
	m_shader->set_param("gPosition", 0);
	m_shader->set_param("gNormal", 1);
	m_shader->set_param("texNoise", 2);
	m_shader->set_param("radius", *debug::get_test_float("radius"));
	m_shader->set_param("bias",   *debug::get_test_float("bias"));
	m_shader->set_param("testA",  *debug::get_test_float("testA"));
	m_shader->set_param("testB",  *debug::get_test_float("testB"));
	m_shader->set_param("testC",  *debug::get_test_float("testC"));
	m_shader->set_param("testD",  *debug::get_test_float("testD"));
	send_data_to_shader();

	Buffer::bind(GL_TEXTURE0, "position");
	Buffer::bind(GL_TEXTURE1, "normal");
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
}

void SSAO::generate_noise_texture()
{
	std::vector<glm::vec3> noise;
	noise.reserve(16);
	for (size_t i = 0; i < 16; i++)
	{
		glm::vec3 sample
		{
			s_randomFloat(s_generator) * 2.0f - 1.0f,
				s_randomFloat(s_generator) * 2.0f - 1.0f,
				0.0f
		};

		noise.push_back(sample);
	}

	glGenTextures(1, &m_noiseTexture);
	glBindTexture(GL_TEXTURE_2D, m_noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, noise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glBindTexture(GL_TEXTURE_2D, 0);
}

SSAOBlur::SSAOBlur()
	: EffectPass{ "ssao", render::BufferFormat::red }
{
}

SSAOBlur::~SSAOBlur()
{
	RenderPass::~RenderPass();
}

void SSAOBlur::load_assets()
{
	m_shader = render::ShaderManager::instance().load_and_store
	(
		"ssao_blur",
		{ "assets/shaders/empty_to_quad.vs", "assets/shaders/ssao_blur.fs" }
	);
}

void SSAOBlur::setup_draw()
{
	if (!m_shader || m_shader->is_outdated())
		m_shader = render::ShaderManager::instance().get("ssao_blur");

	m_shader->use_program();
	Buffer::bind(GL_TEXTURE0, "ssao_raw");
	m_shader->set_param("ssao_raw", 0);
}

