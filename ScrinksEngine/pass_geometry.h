#pragma once

#include "pipeline.h"
#include "shader.h"

namespace scrinks::render::pass
{
	class Geometry : public RenderPass
	{
	public:
		Geometry() {}
		~Geometry() override;

	public:
		void init() override;
		void draw() override;

	private:
		void resize(GLsizei width, GLsizei height) override;

	private:
		std::shared_ptr<Shader> m_shader;
	};
}

