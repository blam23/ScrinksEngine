#pragma once

#include "pipeline.h"
#include "render_target.h"
#include "shader.h"

namespace scrinks::render::pass
{
	class ShadowMap : public RenderPass
	{
	public:
		ShadowMap();
		~ShadowMap();

		ShadowMap* init(GLsizei mapSize);
		ShadowMap* send_to_debug_ui();

		void draw() override;
		void tag_outdated();

	public:
		glm::mat4 get_light_space();

	protected:
		std::shared_ptr<Shader> m_shader{ nullptr };
		std::unique_ptr<RenderTarget> m_target{ nullptr };

	private:
		void resize(GLsizei width, GLsizei height) override;
		GLsizei m_size{ 0 };
		bool m_outdated{ true };
	};
}
