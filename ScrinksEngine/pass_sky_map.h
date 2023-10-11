#pragma once

#include "pipeline.h"
#include "pass_environment_map.h"

namespace scrinks::render::pass
{
	class SkyMap : public RenderPass
	{
	public:
		SkyMap();
		~SkyMap() override;

	public:
		void init();
		void draw(float interpolate) override;

	private:
		void resize(GLsizei, GLsizei) override {}

	private:
		std::shared_ptr<render::Shader> m_shader;
		std::shared_ptr<Buffer> m_target;
	};
}
