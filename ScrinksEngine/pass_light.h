#pragma once

#include "pipeline.h"
#include "shader.h"
#include "render_target.h"

namespace scrinks::render::pass
{
	class DeferredLighting : public RenderPass
	{
	public:
		DeferredLighting() {}
		~DeferredLighting() override;

	public:
		void init() override;
		void draw() override;

	private:
		void resize(GLsizei width, GLsizei height) override;

	private:
		std::shared_ptr<Shader> m_shader;
		std::unique_ptr<RenderTarget> m_target{ nullptr };
	};
}

