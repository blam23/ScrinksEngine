#pragma once

#include "instance_renderer.h"
#include "render_target.h"
#include "texture.h"

#include <list>

namespace scrinks::render
{
	struct SpriteInstance
	{
		float x;
		float y;
		float tileIndex;
	};

	class SpriteRenderer
		: public InstanceRenderer<SpriteInstance>
	{
	public:
		SpriteRenderer(std::shared_ptr<Texture> texture, int tilesPerRow, size_t instanceCount);

	public:
		void init();
		void setup_shader() override;

	private:
		int m_tilesPerRow;
		std::shared_ptr<Texture> m_texture;
	};

	namespace pass
	{
		class SpriteRender : public RenderPass
		{
		public:
			SpriteRender();
			~SpriteRender() override;

		public:
			SpriteRender* init(std::string_view bufferName);
			void draw(float interpolate) override;

			void add_renderer(std::shared_ptr<SpriteRenderer>);
			void remove_renderer(std::shared_ptr<SpriteRenderer>);

		private:
			void resize(GLsizei width, GLsizei height) override;

		private:
			std::list<std::shared_ptr<SpriteRenderer>> m_renderers{};
			std::unique_ptr<render::RenderTarget> m_target;
			std::string m_bufferName;
		};
	}
}

