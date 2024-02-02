#pragma once

#include "node_2d.h"
#include "sprite_renderer.h"

namespace scrinks::core::nodes
{
	class Sprite : public Node2D
	{
	public:
		Sprite(Node* parent, float tileIndex, const glm::vec2& position);
		virtual ~Sprite();

		void set_tile_index(float tileIndex) { m_tileIndex = tileIndex; }
		float get_tile_index() const { return m_tileIndex; }
		float* tile_index_ptr() { return &m_tileIndex; }

		static void update_sprite_data(float interpolate);

	protected:
		virtual const std::string_view default_name() const { return "Sprite"; }
		virtual void setup_script_data() override;
		void fill_sprite_data(scrinks::render::SpriteInstance& out, float interpolate) const;

	private:
		std::size_t m_index;
		float m_tileIndex;

		static inline std::vector<Sprite*> s_sprites{};
	};
}
