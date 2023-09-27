#pragma once

#pragma once

#include "pipeline.h"
#include "shader.h"
#include "render_target.h"
#include "gbuffer.h"

#include <random>
#include <glm/ext/vector_float3.hpp>
#include <gl/gl3w.h>

namespace scrinks::render::pass
{
	// A RenderPass template that will render to a buffer
	class EffectPass : public RenderPass
	{
	public:
		virtual ~EffectPass() {};
		void init() override;
		void draw() override;

	protected:
		EffectPass(const std::string& bufferName, const render::BufferFormat& bufferFormat);
		virtual void setup_draw() = 0;
		virtual void load_assets() = 0;

	protected:
		std::shared_ptr<Shader> m_shader{ nullptr };
		std::unique_ptr<RenderTarget> m_target{ nullptr };

	private:
		std::string m_bufferName;
		render::BufferFormat m_bufferFormat;

		void resize(GLsizei width, GLsizei height) override;
	};
}


