#pragma once

#include "pipeline.h"
#include "render_target.h"
#include "shader.h"
#include "texture.h"

#include <array>

namespace scrinks::render::pass
{
	class EnvironmentMap : public RenderPass
	{
	public:
		EnvironmentMap();
		~EnvironmentMap();

		EnvironmentMap* init(GLsizei mapSize);

		void draw(float interpolate) override;
		void tag_outdated();

		GLuint cubeMap() const { return m_cubeMap; }


	protected:
		std::shared_ptr<Shader> m_shader{ nullptr };
		std::shared_ptr<Texture> m_texture{ nullptr };

	private:
		void resize(GLsizei width, GLsizei height) override;
		GLsizei m_size{ 0 };
		bool m_outdated{ true };
		GLuint m_fbo{ 0 };
		GLuint m_cubeMap{ 0 };
	};
}
