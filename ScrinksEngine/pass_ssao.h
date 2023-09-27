#pragma once

#include "pipeline.h"
#include "shader.h"
#include "render_target.h"

#include <random>
#include <glm/ext/vector_float3.hpp>
#include <gl/gl3w.h>

namespace scrinks::render::pass
{
	class SSAO : public RenderPass
	{
	public:
		SSAO() {}
		~SSAO() override;

	public:
		void configure_kernel_count(size_t kernels);
		void init() override;
		void draw() override;

	private:
		void generate_noise_texture();
		void send_data_to_shader();

	private:
		std::vector<glm::vec3> m_kernel{};
		GLuint m_noiseTexture{ 0 };
		size_t m_kernels_requested{ 64 };
		std::shared_ptr<Shader> m_shader{ nullptr };
		std::unique_ptr<RenderTarget> m_target{ nullptr };

		void resize(GLsizei width, GLsizei height) override;
	};
}

