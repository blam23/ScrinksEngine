#pragma once

#include "effect_pass.h"
#include "pipeline.h"
#include "shader.h"
#include "render_target.h"

#include <random>
#include <glm/ext/vector_float3.hpp>
#include "gl.h"

namespace scrinks::render::pass
{
	class SSAO : public EffectPass
	{
	public:
		SSAO();
		~SSAO() override;

	public:
		void configure_kernel_count(size_t kernels);
		void load_assets() override;
		void setup_draw() override;

	private:
		void generate_noise_texture();
		void send_data_to_shader();

	private:
		std::vector<glm::vec3> m_kernel{};
		GLuint m_noiseTexture{ 0 };
		size_t m_kernels_requested{ 64 };
	};

	class SSAOBlur : public EffectPass
	{
	public:
		SSAOBlur();
		~SSAOBlur() override;

	public:
		void load_assets() override;
		void setup_draw() override;
	};
}

